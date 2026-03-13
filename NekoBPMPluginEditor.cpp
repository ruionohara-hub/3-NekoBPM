#include "NekoBPMPluginEditor.h"
#include "NekoBPMPluginProcessor.h"
#include "BinaryData.h" 

//==============================================================================
// 🐈 GIFフレームのロード処理
void NekoBPMAudioProcessorEditor::loadNekoFrames()
{
    nekoFrames.clear();
    const int numFrames = 158; 
    
    for (int i = 1; i <= numFrames; ++i)
    {
        juce::String resourceName = juce::String::formatted ("neko_%03d_png", i);
        int dataSize;
        const char* data = BinaryData::getNamedResource (resourceName.toRawUTF8(), dataSize);
        
        if (data != nullptr)
        {
            juce::MemoryInputStream is (data, dataSize, false);
            juce::Image image = juce::ImageFileFormat::loadFrom (is);
            
            if (!image.isNull())
                nekoFrames.push_back(std::move(image));
            else
                break; 
        }
        else
        {
            break;
        }
    }
}

//==============================================================================
NekoBPMAudioProcessorEditor::NekoBPMAudioProcessorEditor (NekoBPMAudioProcessor& owner)
    : AudioProcessorEditor (&owner),
      processor (owner)
{
    // 🐈 フレームのロードを実行
    loadNekoFrames(); 

    // **UI要素とアタッチメントの初期化、リスナー登録をすべて削除**

    // タイマーを開始 (初期値 16ms = 約 60 FPS)
    startTimer (16); 

    // ウィンドウサイズの設定
    setSize (250, 350); 
}

NekoBPMAudioProcessorEditor::~NekoBPMAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void NekoBPMAudioProcessorEditor::paint (juce::Graphics& g)
{
    // 背景色を設定
    g.fillAll (juce::Colour::fromRGB (40, 40, 45)); 

    // タイトルの描画
    g.setColour (juce::Colours::white.withAlpha (0.9f));
    g.setFont (juce::Font ("Helvetica", 32.0f, juce::Font::bold));
    g.drawFittedText ("Neko BPM", getLocalBounds().removeFromTop (50).reduced (10, 0), juce::Justification::centred, 2);

    // 🐈 猫のGIFフレームの描画
    auto bounds = getLocalBounds();
    
    // 画像エリアを確定
    auto imageArea = bounds.withSizeKeepingCentre(220, 220).withY(50);
    
    if (!nekoFrames.empty())
    {
        // 枠にきっちり収める描画
        g.drawImage (nekoFrames[currentFrameIndex], 
                     imageArea.toFloat()); 
    }
    else
    {
        g.setColour (juce::Colours::red);
        g.drawFittedText("Error: Neko GIF not loaded", imageArea, juce::Justification::centred, 1);
    }
    
    // 1. 製作者名の描画 (最下部 20px の右側に配置 - 元の位置)
    auto producerArea = bounds.removeFromBottom(20); 
    g.setColour (juce::Colours::white.withAlpha (0.4f));
    g.setFont (12.0f);
    g.drawFittedText ("Producer : suzuya", producerArea.reduced(10, 0), juce::Justification::centredRight, 1);

    // 2. DAW BPM表示
    auto bpmArea = getLocalBounds().removeFromBottom(70); 
    bpmArea.removeFromBottom(20);
    
    juce::String bpmText = "DAW BPM: " + juce::String (processor.currentDAWBPM, 1);
    g.setColour (juce::Colours::cyan);
    g.setFont (juce::Font ("Helvetica", 20.0f, juce::Font::bold));
    g.drawFittedText (bpmText, bpmArea.reduced(10, 0), juce::Justification::centred, 1);
    
    // 3. 再生状態の表示
    auto playStateArea = getLocalBounds().removeFromBottom(55); // BPM表示の下
    juce::String playStateText = processor.isPlaying ? "PLAYING" : "STOPPED";
    g.setColour (processor.isPlaying ? juce::Colours::lime : juce::Colours::orange);
    g.setFont (juce::Font ("Helvetica", 10.0f, juce::Font::bold));
    g.drawFittedText ("State: " + playStateText, playStateArea.reduced(10, 0), juce::Justification::centred, 1);
    // --------------------------
}

void NekoBPMAudioProcessorEditor::resized()
{
    // UI要素を削除済み
}

//==============================================================================
// DAWのBPMに同期してアニメーション速度を調整するコアロジック
void NekoBPMAudioProcessorEditor::timerCallback()
{
    if (! processor.isPlaying) 
    {
        repaint(); 
        return; 
    }
    
    // 1. パラメータ値の取得
    float dawBPM = processor.currentDAWBPM;
    
    // nekobpmは常に120として扱う
    const float baseBPM = 120.0f;
    
    // 2. 再生速度の計算
    float speedRatio = juce::jlimit (0.25f, 4.0f, dawBPM / baseBPM); 
    
    // 3. 必要なフレームレートの計算 
    float targetFPS = originalNekoFPS * speedRatio;
    
    // 4. タイマーの間隔を更新
    float targetIntervalMs = 1000.0f / targetFPS;
    targetIntervalMs = juce::jmax (10.0f, targetIntervalMs); 
    
    startTimer ((int)targetIntervalMs); 

    // 5. フレームの更新と再描画
    if (!nekoFrames.empty())
    {
        currentFrameIndex = (currentFrameIndex + 1) % nekoFrames.size();
        repaint();
    }
}