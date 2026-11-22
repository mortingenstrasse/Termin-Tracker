// High-level track assignment, sample browser, sampler/granular and FX pages.
int fxTab=0;
fs::path browserDir;
struct BrowserEntry{fs::path path;bool directory;};
std::vector<BrowserEntry> browserEntries;
int browserPreview=-1, browserReturn=SAMPLER;
std::string browserStatus;
const char* fxTabs[]={"REVERB","DELAY","SLICER","EQ","SENDS"};
const char* sampleModes[]={"ONE SHOT","FORWARD LOOP","REVERSE","PING PONG","SLICE","GRANULAR"};
const char* grainShapes[]={"SQUARE","TRIANGLE","GAUSSIAN","HANN"};
const char* grainDirections[]={"FORWARD","REVERSE","PING PONG"};
const char* lfoNames[]={"OFF","SYNC","FREE"};
const char* lfoShapes[]={"TRIANGLE","SINE","SAW UP","SAW DOWN","SQUARE","RANDOM"};
const char* scanNames[]={"FULL RANGE","ANCHOR STEPS","ANCHOR SWEEP"};
int trackSlot(){return pat().instruments[track];}
bool validSlot(int i){return i>=0&&i<slots&&project.instruments[i].name!="EMPTY";}
bool hasSample(int i){return validSlot(i)&&!project.instruments[i].pcm.empty();}
std::string trackTitle(){int i=trackSlot();return "T"+num(track+1)+"  "+(validSlot(i)?num(i+1)+" "+project.instruments[i].name:"NO INSTRUMENT");}
void assignTrack(int i){
    if(!validSlot(i))return;
    pat().instruments[track]=i;
    // A track assignment replaces the previous track sound, including old rows
    // that stored the old instrument explicitly. Leave note/FX data intact.
    for(int s=0;s<steps;s++){pat().cells[track][s].instrument=-1;publishCell(project.selected,track,s);}
    dirty=true;message="T"+num(track+1)+" ASSIGNED "+project.instruments[i].name;
}
void previewSlot(int i){if(!validSlot(i)){message="NO INSTRUMENT";return;}int n=hasSample(i)?project.instruments[i].root:editNote;tm_preview(pack({n}),i);}
void switchTrack(int delta){tm_preview_stop();track=clamp(track+delta,0,tracks-1);focus=page==INSTRUMENT?std::max(0,trackSlot()):0;message=trackTitle();}
bool isVisibleEntry(const fs::path& p){auto s=p.filename().string();return !s.empty()&&s[0]!='.';}
bool isWav(const fs::path& p){auto s=p.extension().string();std::transform(s.begin(),s.end(),s.begin(),[](unsigned char x){return char(std::tolower(x));});return isVisibleEntry(p)&&s==".wav";}
std::vector<BrowserEntry> scanFolder(const fs::path& root){std::vector<BrowserEntry> list;std::error_code e;for(fs::directory_iterator it(root,e),end;!e&&it!=end;it.increment(e)){const auto& p=it->path();if(!isVisibleEntry(p))continue;std::error_code typeError;bool dir=it->is_directory(typeError);if(dir||(!typeError&&it->is_regular_file(typeError)&&isWav(p)))list.push_back({p,dir});}std::sort(list.begin(),list.end(),[](auto& a,auto& b){return a.directory!=b.directory?a.directory>b.directory:a.path.filename()<b.path.filename();});return list;}
int emptySampleSlot(){for(int i=10;i<slots;i++)if(project.instruments[i].name=="EMPTY")return i;return -1;}
void stopBrowserPreview(){if(browserPreview>=0){tm_preview_stop();tm_instrument_clear(browserPreview);browserPreview=-1;}}
void refreshBrowser(){stopBrowserPreview();browserEntries=scanFolder(browserDir);waveIndex=0;browserStatus=browserEntries.empty()?"EMPTY FOLDER":"A OPEN / LOAD   Y AUDITION   B BACK";}
void openBrowser(int from){browserReturn=from;browserDir=dataRoot/"Sample Set";refreshBrowser();modal=6;}
void closeBrowser(){stopBrowserPreview();modal=0;browserStatus.clear();}
void sampleBrowserInput(Button b){
    if(b==UP||b==DOWN){stopBrowserPreview();waveIndex=clamp(waveIndex+(b==UP?-1:1),0,std::max(0,int(browserEntries.size())-1));browserStatus="A OPEN / LOAD   Y AUDITION   B BACK";return;}
    if(b==B||b==LEFT){if(browserDir!=dataRoot/"Sample Set"){auto prior=browserDir;browserDir=browserDir.parent_path();refreshBrowser();for(int i=0;i<(int)browserEntries.size();i++)if(browserEntries[i].path==prior)waveIndex=i;}else closeBrowser();return;}
    if(b==X){closeBrowser();return;}
    if(browserEntries.empty())return;auto entry=browserEntries[waveIndex];
    if(entry.directory){if(b==A||b==RIGHT){browserDir=entry.path;refreshBrowser();}return;}
    if(b==Y){if(browserPreview>=0&&tm_preview_active()){stopBrowserPreview();browserStatus="PREVIEW STOPPED";return;}stopBrowserPreview();Slot s;std::string err;int i=emptySampleSlot();if(i<0){browserStatus="NO FREE SAMPLE SLOT";return;}
        if(!decodeWav(entry.path,s,err)||!tm_instrument_sample(i,s.pcm.data(),s.pcm.size(),s.sampleRate,s.root)){browserStatus=err.empty()?"AUDIO ENGINE REJECTED WAV":err;return;}browserPreview=i;tm_instrument_wave(i,2);tm_sample_config_scan(i,0,0,s.end,nullptr,0,500,120,3,0,0,0,0,3,100,0,nullptr);tm_sample_octave(i,0);tm_preview(pack({s.root}),i);browserStatus="AUDITION: "+s.name;return;}
    if(b==A||b==RIGHT){stopBrowserPreview();int i=emptySampleSlot();if(i<0){browserStatus="NO FREE SAMPLE SLOT";return;}std::string err;if(!importWav(entry.path,i,err)){browserStatus=err;return;}
        assignTrack(i);if(browserReturn==GRANULAR){project.instruments[i].mode=5;publishSampleConfig(i);}page=browserReturn==GRANULAR?GRANULAR:SAMPLER;focus=0;closeBrowser();previewSlot(i);message="LOADED T"+num(track+1)+" / "+project.instruments[i].name;}
}
void instrumentInput(Button b){if(b==L1||b==R1){switchTrack(b==L1?-1:1);return;}if(b==UP||b==DOWN)focus=clamp(focus+(b==UP?-1:1),0,slots-1);if(b==A&&validSlot(focus)){assignTrack(focus);previewSlot(focus);}if(b==Y)previewSlot(focus);if(b==X)openBrowser(INSTRUMENT);}
void editOmondeu(Button b){if(b==L1||b==R1){switchTrack(b==L1?-1:1);return;}int i=trackSlot();if(!validSlot(i)||project.instruments[i].wave!=9||hasSample(i)){if(b==A)assignTrack(9);return;}auto& p=project.instruments[i].patch;if(b==UP||b==DOWN)focus=clamp(focus+(b==UP?-1:1),0,22);if(b==LEFT||b==RIGHT){p[focus]=clamp(p[focus]+omStep[focus]*(b==LEFT?-1:1)*(shift?4:1),omLo[focus],omHi[focus]);tm_omondeu(i,p.data());dirty=true;}if(b==Y)previewSlot(i);}
int sampleParamCount(bool granular){return granular?18:7;}
void sampleEditorInput(Button b,bool granular){
    if(b==L1||b==R1){switchTrack(b==L1?-1:1);return;}int i=trackSlot();if(b==X){openBrowser(granular?GRANULAR:SAMPLER);return;}
    if(!hasSample(i)){if(b==A)openBrowser(granular?GRANULAR:SAMPLER);return;}
    auto& s=project.instruments[i];if(granular&&s.mode!=5){if(b==A){s.mode=5;publishSampleConfig(i);dirty=true;previewSlot(i);}else if(b==Y)previewSlot(i);return;}
    if(b==Y){if(tm_preview_active())tm_preview_stop();else previewSlot(i);return;}
    if(b==UP||b==DOWN){focus=clamp(focus+(b==UP?-1:1),0,sampleParamCount(granular)-1);return;}
    if(b==A&&!granular&&focus==6){s.slices.clear();for(int j=0;j<8;j++){int p=s.start+(s.end-s.start)*j/8;if(s.slices.empty()||p>s.slices.back())s.slices.push_back(p);}s.mode=4;publishSampleConfig(i);dirty=true;message="SLICES CREATED";return;}
    int d=(b==LEFT?-1:b==RIGHT?1:0)*(shift?4:1);if(!d)return;
    int sampleStep=std::max(1,int(s.sampleRate/100.));
    if(!granular){switch(focus){case 0:s.mode=clamp(s.mode+d,0,5);break;case 1:s.start=clamp(s.start+d*sampleStep,0,s.end-1);break;case 2:s.end=clamp(s.end+d*sampleStep,s.start+1,int(s.pcm.size()));break;case 3:s.root=clamp(s.root+d,0,127);if(!tm_instrument_sample(i,s.pcm.data(),s.pcm.size(),s.sampleRate,s.root))message="ROOT UPDATE FAILED";break;case 4:s.octave=clamp(s.octave+d,-3,3);break;case 5:s.position=clamp(s.position+d*10,0,1000);break;default:return;}}
    else {switch(focus){case 0:s.position=clamp(s.position+d*10,0,1000);break;case 1:s.length=clamp(s.length+d*5,1,1000);break;case 2:s.shape=clamp(s.shape+d,0,3);break;case 3:s.direction=clamp(s.direction+d,0,2);break;case 4:s.spread=clamp(s.spread+d*2,0,100);break;case 5:s.octave=clamp(s.octave+d,-3,3);break;case 6:s.lfoMode=clamp(s.lfoMode+d,0,2);break;case 7:s.lfoShape=clamp(s.lfoShape+d,0,5);break;case 8:if(s.lfoMode==2)s.lfoFree=clamp(s.lfoFree+d*5,5,2000);else s.lfoSpeed=clamp(s.lfoSpeed+d,0,28);break;case 9:s.lfoDepth=clamp(s.lfoDepth+d*5,0,100);break;case 10:s.scanMode=clamp(s.scanMode+d,0,2);break;case 11:s.start=clamp(s.start+d*sampleStep,0,s.end-1);break;case 12:s.end=clamp(s.end+d*sampleStep,s.start+1,int(s.pcm.size()));break;default:s.anchors[focus-13]=clamp(s.anchors[focus-13]+d*10,0,1000);break;}}
    publishSampleConfig(i);dirty=true;
}
int fxCount(){return std::array<int,5>{7,6,1,4,3}[fxTab];}
void fxInput(Button b){if(b==L1||b==R1){if(shift){switchTrack(b==L1?-1:1);return;}fxTab=(fxTab+(b==L1?4:1))%5;focus=0;return;}if(b==UP||b==DOWN){focus=clamp(focus+(b==UP?-1:1),0,fxCount()-1);return;}if(b!=LEFT&&b!=RIGHT)return;int d=(b==LEFT?-1:1)*(shift?5:1);
    if((fxTab==0&&focus<6)||(fxTab==1&&focus<5)){int f=fxTab==0?focus:focus+6;const int hi[]={100,120,250,100,150,100,6,90,100,100,1};project.fx[f]=clamp(project.fx[f]+d,f==1?1:0,hi[f]);publishFX();}
    else if(fxTab==3){pat().eq[track][focus]=clamp(pat().eq[track][focus]+d,-18,18);publishMix(project.selected);}
    else{int i=fxTab==4?focus:fxTab;pat().sends[track][i]=clamp(pat().sends[track][i]+d,0,100);publishMix(project.selected);}dirty=true;
}

void drawBrowser(){box(0,0,640,480,black);box(0,0,640,55,panel);text(12,8,"SAMPLE BROWSER  T"+num(track+1),white,2);std::error_code e;auto rel=fs::relative(browserDir,dataRoot/"Sample Set",e);text(12,33,rel=="."?"SAMPLE SET":"SAMPLE SET / "+(e?browserDir.filename().string():rel.string()),cyan,1,95);int first=std::max(0,std::min(waveIndex-5,int(browserEntries.size())-12));for(int j=first;j<std::min(first+12,int(browserEntries.size()));j++){int y=65+(j-first)*29;auto& a=browserEntries[j];box(12,y,616,26,j==waveIndex?selection:panel);if(j==waveIndex)border(12,y,616,26,green);text(20,y+6,a.directory?">":"W",a.directory?olive:cyan,2);text(45,y+5,a.path.filename().string(),white,2,47);}if(browserEntries.empty())center(0,215,640,"NO WAV FILES OR FOLDERS",olive);box(0,420,640,60,panel);text(12,427,browserStatus,white,1,100);text(12,448,"A OPEN/LOAD  Y AUDITION  B BACK  X CANCEL",cyan,1);}
void drawTrackBand(const std::string& title){box(0,34,640,32,panel);text(10,40,title,cyan,2,51);}
void drawInstrumentPage(){drawTrackBand(trackTitle());int first=clamp(focus-5,0,slots-12);for(int j=first;j<first+12;j++){int y=72+(j-first)*30;box(12,y,616,27,j==focus?selection:panel);if(j==focus)border(12,y,616,27,green);text(20,y+5,num(j+1)+" "+(hasSample(j)&&project.instruments[j].mode==5?"G ":"")+project.instruments[j].name,validSlot(j)?white:line,2,39);if(j==trackSlot())text(510,y+9,"ASSIGNED",cyan,1);}if(message.empty())message="A ASSIGN  X BROWSE WAV  Y AUDITION  L1/R1 TRACK";}
std::array<std::array<float,600>,slots> wavePeaks{};std::array<unsigned,slots> waveCachedRevision{};
void drawSampleWave(int i,bool granular){auto& s=project.instruments[i];int n=int(s.pcm.size());if(n<=0)return;auto& peaks=wavePeaks[i];if(waveCachedRevision[i]!=sampleRevision[i]){for(int x=0;x<600;x++){size_t a=size_t(x)*n/600,b=std::max(a+1,size_t(x+1)*n/600);float peak=0;for(size_t k=a;k<std::min<size_t>(b,n);k++)peak=std::max(peak,std::abs(s.pcm[k]));peaks[x]=peak;}waveCachedRevision[i]=sampleRevision[i];}box(20,72,600,112,panel);box(20,127,600,1,line);for(int x=0;x<600;x++){int h=int(peaks[x]*53);box(20+x,127-h,1,h*2+1,white);}int start=20+s.start*599/n,end=20+s.end*599/n;box(start,72,2,112,recCol);box(end,72,2,112,green);if(granular){int pos=start+(end-start)*(s.lfoMode?tm_sample_position(i):s.position)/1000;box(pos,72,2,112,cyan);if(s.scanMode)for(int a:s.anchors)box(start+(end-start)*a/1000,72,1,112,olive);}text(22,188,std::to_string(int(s.sampleRate))+" HZ / "+std::to_string(n)+" FRAMES",olive,1);}
std::string ms(int frames,double sr){return std::to_string(int(frames*1000./sr))+" MS";}
void drawParamList(const std::vector<std::pair<std::string,std::string>>& rows,int top,int count){int first=clamp(focus-count/2,0,std::max(0,int(rows.size())-count));for(int j=first;j<std::min(first+count,int(rows.size()));j++){int y=top+(j-first)*30;box(20,y,600,27,j==focus?selection:panel);if(j==focus)border(20,y,600,27,green);text(30,y+5,rows[j].first,white,2,30);text(405,y+5,rows[j].second,olive,2,17);}}
void drawSamplePage(bool granular){drawTrackBand((granular?"GRANULAR  ":"SAMPLER  ")+trackTitle());int i=trackSlot();if(!hasSample(i)){center(0,180,640,"NO SAMPLE ON THIS TRACK",white,2);center(0,225,640,"A OR X - BROWSE WAV",cyan,2);message="L1/R1 CHANGE TRACK  A/X BROWSE";return;}auto& s=project.instruments[i];drawSampleWave(i,granular);if(granular&&s.mode!=5){center(0,265,640,"A ENABLE GRANULAR",cyan,2);center(0,310,640,"Y AUDITION ORIGINAL SAMPLE",olive,1);message="A ENABLE  X LOAD WAV  L1/R1 TRACK";return;}
    std::vector<std::pair<std::string,std::string>> r;if(!granular)r={{"PLAYBACK",sampleModes[s.mode]},{"START",ms(s.start,s.sampleRate)},{"END",ms(s.end,s.sampleRate)},{"ROOT NOTE",noteName(s.root)},{"OCTAVE",std::to_string(s.octave)},{"POSITION",std::to_string(s.position/10)+"%"},{"CHOP 8 SLICES","A APPLY"}};
    else{r={{"POSITION",std::to_string(s.position/10)+"%"},{"GRAIN LENGTH",std::to_string(s.length)+" MS"},{"SHAPE",grainShapes[s.shape]},{"DIRECTION",grainDirections[s.direction]},{"SPREAD",std::to_string(s.spread)+"%"},{"OCTAVE",std::to_string(s.octave)},{"LFO MODE",lfoNames[s.lfoMode]},{"LFO SHAPE",lfoShapes[s.lfoShape]},{"LFO SPEED",s.lfoMode==2?std::to_string(s.lfoFree)+" / 100 HZ":std::to_string(lfoRates[clamp(s.lfoSpeed,0,28)])+" TICKS"},{"LFO DEPTH",std::to_string(s.lfoDepth)+"%"},{"SCAN MODE",scanNames[s.scanMode]},{"START",ms(s.start,s.sampleRate)},{"END",ms(s.end,s.sampleRate)}};for(int j=0;j<5;j++)r.push_back({"ANCHOR "+num(j+1),std::to_string(s.anchors[j]/10)+"%"});}
    drawParamList(r,212,7);if(message.empty())message="X LOAD WAV  Y AUDITION  L1/R1 TRACK  ARROWS EDIT";
}
void drawFXPage(){drawTrackBand("FX  "+trackTitle());for(int j=0;j<5;j++){int x=8+j*126;box(x,74,122,30,j==fxTab?selection:panel);if(j==fxTab)border(x,74,122,30,green);center(x,81,122,fxTabs[j],j==fxTab?white:olive,2);}std::vector<std::pair<std::string,std::string>> r;
    if(fxTab==0){for(int j=0;j<6;j++)r.push_back({fxLabels[j],j==1?std::to_string(project.fx[j]/10)+"."+std::to_string(project.fx[j]%10)+" S":j==2?std::to_string(project.fx[j])+" MS":std::to_string(project.fx[j])+"%"});r.push_back({"TRACK REVERB SEND",std::to_string(pat().sends[track][0])+"%"});}
    else if(fxTab==1){for(int j=6;j<11;j++)r.push_back({fxLabels[j],j==6?std::array<std::string,7>{"1/32","1/16","1/8","1/8 DOT","1/4","1/2","1 BAR"}[project.fx[j]]:j==10?(project.fx[j]?"ON":"OFF"):std::to_string(project.fx[j])+"%"});r.push_back({"TRACK DELAY SEND",std::to_string(pat().sends[track][1])+"%"});}
    else if(fxTab==2)r={{"TRACK SLICER SEND",std::to_string(pat().sends[track][2])+"%"}};
    else if(fxTab==3){const char* names[]={"BASS 100 HZ","MID A 500 HZ","MID B 2.5 KHZ","HIGH 8 KHZ"};for(int j=0;j<4;j++)r.push_back({names[j],std::to_string(pat().eq[track][j])+" DB"});}
    else{for(int j=0;j<3;j++)r.push_back({std::array<std::string,3>{"REVERB SEND","DELAY SEND","SLICER SEND"}[j],std::to_string(pat().sends[track][j])+"%"});}
    drawParamList(r,120,8);message="L1/R1 FX TAB  UP/DOWN PARAMETER  LEFT/RIGHT EDIT";
}
