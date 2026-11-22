// Integration regression tests; run on both macOS and Linux ARM64 before install.
bool audioEnergy(int blocks,double& energy){std::array<float,256> l{},r{};energy=0;for(int j=0;j<blocks;j++){tm_render(l.data(),r.data(),l.size());for(int i=0;i<(int)l.size();i++){if(!std::isfinite(l[i])||!std::isfinite(r[i]))return false;energy+=double(l[i])*l[i]+double(r[i])*r[i];}}return true;}
bool failTest(const std::string& msg){std::cerr<<"FAIL: "<<msg<<std::endl;return false;}
bool regressionTests(const fs::path& library){
    DirectionRepeat repeat;if(repeat.tick(true,100)||repeat.tick(true,379)||!repeat.tick(true,380)||repeat.tick(true,459)||!repeat.tick(true,460)||repeat.tick(false,461)||repeat.tick(true,462))return failTest("D-pad hold timing and release");
    for(int i=0;i<PAGE_COUNT;i++){input(SELECT,true);menu=i;input(A,true);if(page!=i||modal!=0)return failTest("page chooser "+std::to_string(i));}
    std::cout<<"PASS D-pad repeat and all 9 pages\n";
    resetProject();enterEditor();setPage(INSTRUMENT);focus=9;input(A,true);if(trackSlot()!=9)return failTest("Omondeu track assignment");
    setPage(OMONDEU);focus=8;int cutoff=project.instruments[9].patch[8];input(LEFT,true);if(project.instruments[9].patch[8]>=cutoff)return failTest("Omondeu cutoff");
    setPage(PATTERN);input(L2,true);input(A,true);input(L2,false);input(A,true);if(modal!=2)return failTest("note picker");input(RIGHT,true);input(A,true);if(pat().cells[0][0].notes.size()!=3||row!=1)return failTest("chord entry and step advance");
    row=0;input(R2,true);menu=1;input(A,true);for(int j=0;j<8;j++)input(RIGHT,true);input(A,true);if(cell().duration!=8)return failTest("duration edit");
    input(R2,true);menu=3;input(A,true);input(LEFT,true);input(A,true);if(pat().length!=63)return failTest("pattern length");
    input(R2,true);menu=5;input(A,true);input(RIGHT,true);input(A,true);if(project.bpm!=1210)return failTest("tempo edit");
    input(R2,true);menu=4;input(A,true);if(project.selected!=1||pat().cells[0][0].duration!=8)return failTest("copy pattern and note data");
    track=1;input(R2,true);menu=0;input(A,true);fillRole=2;fillPreset=2;input(A,true);int hits=0;for(auto& c:pat().cells[1])if(!c.notes.empty())hits++;if(hits==0||hits>=pat().length||trackSlot()!=1)return failTest("creative fill");
    setPage(SONG);input(Y,true);if(project.song.size()!=2)return failTest("song insert");input(RIGHT,true);if(!project.patterns.count(project.song[songRow]))return failTest("song pattern selection");
    std::cout<<"PASS Omondeu controls, chord entry, duration, tempo, pattern copy, fill and song chain\n";
    auto root=library/"Sample Set";auto cats=scanFolder(root);std::vector<fs::path> files;
    for(auto& cat:cats)if(cat.directory)for(auto& file:scanFolder(cat.path))if(!file.directory)files.push_back(file.path);
    if(cats.size()<6||files.size()<60)return failTest("factory categories or samples missing: "+std::to_string(cats.size())+" / "+std::to_string(files.size()));
    resetProject();enterEditor();double energy=0;audioEnergy(64,energy);int audited=0;
    for(auto& file:files){std::string err;if(!importWav(file,10,err))return failTest(file.string()+": "+err);auto& s=project.instruments[10];if(s.pcm.empty()||s.end!=int(s.pcm.size())||!std::all_of(s.pcm.begin(),s.pcm.end(),[](float x){return std::isfinite(x);}))return failTest("decoded PCM: "+file.string());
        tm_preview_stop();audioEnergy(4,energy);previewSlot(10);if(!audioEnergy(32,energy)||energy<1e-8)return failTest("silent or nonfinite preview: "+file.string());audited++;}
    resetProject();enterEditor();dataRoot=library;setPage(INSTRUMENT);input(X,true);if(modal!=6||browserEntries.size()<6)return failTest("instrument opens folder browser");
    auto kick=std::find_if(browserEntries.begin(),browserEntries.end(),[](auto& x){return x.path.filename().string().find("Kick")!=std::string::npos;});if(kick==browserEntries.end())return failTest("kick folder");waveIndex=int(kick-browserEntries.begin());input(A,true);if(browserEntries.empty()||browserEntries.front().directory)return failTest("folder navigation");
    for(auto& x:browserEntries)if(!isVisibleEntry(x.path)||!isWav(x.path))return failTest("hidden metadata leaked to browser");
    pat().cells[0][0].notes={36};pat().cells[0][0].instrument=0;pat().instruments[0]=0;input(Y,true);if(browserPreview<0||!audioEnergy(20,energy)||energy<=0)return failTest("browser audition");input(A,true);int loaded=trackSlot();if(modal||page!=SAMPLER||!hasSample(loaded)||pat().cells[0][0].instrument!=-1||pat().cells[0][0].notes!=std::vector<int>{36})return failTest("sample load and track-wide reassignment");
    tm_preview_stop();audioEnergy(32,energy);tm_play(1);tm_song_play(0);tm_start_step(0);if(!audioEnergy(64,energy)||energy<=0)return failTest("sample sequence output");tm_play(0);
    int prev=track;input(R1,true);if(track!=prev+1||trackSlot()!=-1)return failTest("sampler track context");input(L1,true);if(trackSlot()!=loaded)return failTest("sampler returns correct slot");
    focus=6;input(A,true);if(project.instruments[loaded].mode!=4||project.instruments[loaded].slices.size()!=8)return failTest("chop 8");
    std::cout<<"PASS "<<audited<<" WAV files decoded and audibly rendered; browser, assignment, sequencing, chop\n";
    setPage(GRANULAR);input(X,true);auto synth=std::find_if(browserEntries.begin(),browserEntries.end(),[](auto& x){return x.path.filename().string().find("Synth")!=std::string::npos;});if(synth==browserEntries.end())return failTest("synth category");waveIndex=int(synth-browserEntries.begin());input(A,true);if(browserEntries.empty())return failTest("synth files");waveIndex=0;input(A,true);int g=trackSlot();if(!hasSample(g)||project.instruments[g].mode!=5||page!=GRANULAR||modal!=0)return failTest("granular load direct");
    auto& s=project.instruments[g];s.length=45;s.lfoMode=2;s.lfoFree=200;s.scanMode=1;s.anchors={100,275,500,700,900};publishSampleConfig(g);
    tm_preview_stop();audioEnergy(5,energy);tm_preview(pack({s.root,s.root+3,s.root+7}),g);std::vector<int> observed;double total=0;for(int j=0;j<180;j++){if(!audioEnergy(1,energy))return failTest("granular finite output");total+=energy;int p=tm_sample_position(g);if(std::find(observed.begin(),observed.end(),p)==observed.end())observed.push_back(p);}
    if(total<1e-8||observed.size()<2||!std::all_of(observed.begin(),observed.end(),[&](int p){return std::find(s.anchors.begin(),s.anchors.end(),p)!=s.anchors.end();}))return failTest("granular polyphonic output or LFO anchor stepping");
    std::cout<<"PASS granular polyphony and LFO anchor stepping ("<<observed.size()<<" positions)\n";
    setPage(FX);fxTab=0;focus=0;int old=project.fx[0];input(RIGHT,true);if(project.fx[0]!=old+1)return failTest("reverb setting");input(R1,true);if(fxTab!=1||focus!=0)return failTest("delay tab");input(RIGHT,true);input(R1,true);if(fxTab!=2)return failTest("slicer tab");input(RIGHT,true);if(pat().sends[track][2]!=1)return failTest("slicer send");input(R1,true);input(RIGHT,true);if(fxTab!=3||pat().eq[track][0]!=1)return failTest("EQ tab");input(R1,true);input(RIGHT,true);if(fxTab!=4||pat().sends[track][0]!=1)return failTest("sends tab");input(L1,true);if(fxTab!=3)return failTest("reverse tab");
    setPage(MIXER);input(A,true);input(X,true);if(!(pat().mute&1)||!(pat().solo&1))return failTest("mixer mute and solo");input(A,true);input(X,true);input(Y,true);if(page!=FX||fxTab!=4)return failTest("mixer sends navigation");
    auto path=fs::temp_directory_path()/("termin-pocket-regression-"+std::to_string(SDL_GetTicks())+".trackermac");std::string err;if(!save(path)||!load(path,err))return failTest("sample project roundtrip: "+err);std::error_code ec;fs::remove(path,ec);
    if(!hasSample(g)||project.instruments[g].mode!=5||project.instruments[g].anchors[3]!=700||pat().instruments[0]!=g||pat().eq[0][0]!=1)return failTest("project persistence values");
    std::cout<<"PASS FX tabs, sends, mixer, sample/granular/FX project persistence\n";
    return true;
}
bool uiSmokeTest(){
    auto* surface=SDL_CreateRGBSurfaceWithFormat(0,640,480,32,SDL_PIXELFORMAT_RGBA32);if(!surface)return failTest("software UI surface");
    renderer=SDL_CreateSoftwareRenderer(surface);if(!renderer){SDL_FreeSurface(surface);return failTest("software UI renderer");}initFont();
    auto visible=[&](const std::string& name){int pixels=0;auto* data=static_cast<unsigned char*>(surface->pixels);for(int y=0;y<480;y++)for(int x=0;x<640;x++){unsigned char* p=data+y*surface->pitch+x*4;if(p[0]||p[1]||p[2])pixels++;}if(pixels<500)return failTest("blank UI: "+name);if(const char* out=std::getenv("TERMIN_POCKET_TEST_IMAGES"))SDL_SaveBMP(surface,(fs::path(out)/(name+".bmp")).c_str());return true;};
    bool passed=true;track=0;bootPage=BootPage::Editor;modal=0;
    for(int i=0;i<PAGE_COUNT;i++){setPage(i);if(i==FX){for(int j=0;j<5;j++){fxTab=j;focus=0;color(black);SDL_RenderClear(renderer);drawPage();passed=visible(std::string("FX-")+fxTabs[j])&&passed;}}else{color(black);SDL_RenderClear(renderer);drawPage();passed=visible(pageNames[i])&&passed;}}
    setPage(SAMPLER);input(X,true);color(black);SDL_RenderClear(renderer);drawPage();passed=visible("SAMPLE-FOLDERS")&&passed;input(A,true);color(black);SDL_RenderClear(renderer);drawPage();passed=visible("SAMPLE-FILES")&&passed;closeBrowser();
    openHome();color(black);SDL_RenderClear(renderer);drawStartup();passed=visible("HOME")&&passed;
    SDL_DestroyTexture(font);SDL_DestroyRenderer(renderer);SDL_FreeSurface(surface);renderer=nullptr;font=nullptr;
    if(passed)std::cout<<"PASS software-rendered UI: nine pages, five FX tabs, sample folders/files, home\n";return passed;
}
bool fxSignalTest(){
    auto render=[&](int mode,std::array<double,2>& result){
        double drain=0;tm_preview_stop();tm_play(0);audioEnergy(64,drain);tm_init(rate);resetProject();enterEditor();
        auto& s=project.instruments[10];s.name="TEST TONE";s.wave=2;s.root=60;s.sampleRate=rate;s.pcm.resize(rate/5);s.end=s.pcm.size();
        for(size_t i=0;i<s.pcm.size();i++)s.pcm[i]=float(.25*std::sin(6.283185307179586*60*i/rate)*std::min(1.,double(s.pcm.size()-i)/480.));
        publishSlot(10);assignTrack(10);cell().notes={60};publishCell(0,0,0);
        if(mode==1){pat().sends[0][0]=100;project.fx[1]=20;project.fx[5]=100;}
        if(mode==2){pat().sends[0][1]=100;project.fx[6]=4;project.fx[7]=65;project.fx[9]=100;}
        if(mode==3)pat().sends[0][2]=100;
        if(mode==4)pat().eq[0][0]=-18;
        publishFX();publishMix(0);tm_dynamics(0,0,0,-24,3,5,150,0,-30,6);tm_song_play(0);tm_start_step(0);tm_play(1);
        std::array<float,256> l{},r{};result={0,0};for(int block=0;block<280;block++){tm_render(l.data(),r.data(),l.size());for(int i=0;i<256;i++){if(!std::isfinite(l[i])||!std::isfinite(r[i]))return false;double e=double(l[i])*l[i]+double(r[i])*r[i];if(block<38)result[0]+=e;if(block>70)result[1]+=e;}}tm_play(0);return true;
    };
    std::array<double,2> dry{},reverb{},delay{},slicer{},eq{};
    if(!render(0,dry)||!render(1,reverb)||!render(2,delay)||!render(3,slicer)||!render(4,eq)||dry[0]<=0)return failTest("FX audio finite output");
    if(reverb[1]<dry[1]+1e-5||delay[1]<dry[1]+1e-5||slicer[0]>=dry[0]*.95||eq[0]>=dry[0]*.95)return failTest("FX audio difference from dry signal");
    std::cout<<"PASS actual DSP signal: reverb and delay tails, slicer gating, bass EQ attenuation\n";return true;
}
