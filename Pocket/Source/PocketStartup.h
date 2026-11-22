// Included after the project, rendering and editor definitions in Pocket.cpp.
// Use physical SDL joystick inputs on the handheld, independent of the OS's
// controller database (which can assign different meanings to the same GUID).
enum class BootPage { Splash, Controls, Home, Load, Confirm, Editor };
BootPage bootPage=BootPage::Splash;
Uint32 bootAt=0;
bool handheld=false, activeProject=false;
int homeChoice=0, loadChoice=1, pendingChoice=0;
std::string bootMessage;
SDL_Joystick* physicalPad=nullptr;
std::array<int,BUTTON_COUNT> physicalMap{};
std::array<bool,BUTTON_COUNT> physicalHeld{};
struct DirectionRepeat {Uint32 started=0,next=0;bool active=false;bool tick(bool held,Uint32 now){if(!held){active=false;return false;}if(!active){active=true;started=now;next=now+280;return false;}if(Sint32(now-next)<0)return false;next=now+(now-started>=1200?45:80);return true;}};
std::array<DirectionRepeat,4> directionRepeat{};
std::vector<int> axisIdle;
int calibrationIndex=0;
bool calibrationReleased=false;
const char* physicalNames[]={"D-PAD UP","D-PAD DOWN","D-PAD LEFT","D-PAD RIGHT","A - RIGHT","B - BOTTOM","X - TOP","Y - LEFT","L1","R1","L2","R2","START","SELECT","MENU"};

std::string padIdentity(){return physicalPad&&SDL_JoystickName(physicalPad)?SDL_JoystickName(physicalPad):"";}
std::vector<int> pressedPhysical(){
    std::vector<int> out;if(!physicalPad)return out;
    for(int i=0;i<SDL_JoystickNumButtons(physicalPad)&&i<256;i++)if(SDL_JoystickGetButton(physicalPad,i))out.push_back(i);
    const int hats[]={SDL_HAT_UP,SDL_HAT_DOWN,SDL_HAT_LEFT,SDL_HAT_RIGHT};
    for(int i=0;i<SDL_JoystickNumHats(physicalPad)&&i<64;i++)for(int j=0;j<4;j++)if(SDL_JoystickGetHat(physicalPad,i)&hats[j])out.push_back(256+i*4+j);
    for(int i=0;i<(int)axisIdle.size();i++){int delta=int(SDL_JoystickGetAxis(physicalPad,i))-axisIdle[i];if(delta>20000)out.push_back(512+i*2);if(delta<-20000)out.push_back(513+i*2);}
    return out;
}
bool readControls(){
    std::ifstream f(dataRoot/"controls-v2.txt");std::string name;std::getline(f,name);if(name!=padIdentity())return false;
    for(int i=0;i<BUTTON_COUNT;i++)if(!(f>>physicalMap[i])||physicalMap[i]<-1||physicalMap[i]>=1024||(i!=MENU&&physicalMap[i]<0))return false;
    for(int i=0;i<BUTTON_COUNT;i++)for(int j=0;j<i;j++)if(physicalMap[i]>=0&&physicalMap[i]==physicalMap[j])return false;
    return true;
}
void writeControls(){std::ofstream f(dataRoot/"controls-v2.txt.tmp");f<<padIdentity()<<'\n';for(int n:physicalMap)f<<n<<' ';f.close();if(f){std::error_code e;fs::rename(dataRoot/"controls-v2.txt.tmp",dataRoot/"controls-v2.txt",e);}}
void beginCalibration(){physicalMap.fill(-1);physicalHeld.fill(false);calibrationIndex=0;calibrationReleased=false;bootPage=BootPage::Controls;bootMessage.clear();}
void openPhysicalPad(){
    for(int i=0;i<SDL_NumJoysticks();i++){
        const char* n=SDL_JoystickNameForIndex(i);std::string name=n?n:"";
        std::transform(name.begin(),name.end(),name.begin(),[](unsigned char c){return char(std::tolower(c));});
        if(name.find("anbernic")!=std::string::npos||name.find("rg35xx")!=std::string::npos){physicalPad=SDL_JoystickOpen(i);if(physicalPad)break;}
    }
    if(!physicalPad&&SDL_NumJoysticks()==1)physicalPad=SDL_JoystickOpen(0);
    if(!physicalPad)return;
    SDL_JoystickUpdate();for(int i=0;i<SDL_JoystickNumAxes(physicalPad)&&i<128;i++)axisIdle.push_back(SDL_JoystickGetAxis(physicalPad,i));
    std::cout<<"Physical controller: "<<padIdentity()<<" buttons="<<SDL_JoystickNumButtons(physicalPad)<<" hats="<<SDL_JoystickNumHats(physicalPad)<<std::endl;
}
void pollPhysical(){
    if(!physicalPad||bootPage==BootPage::Splash)return;
    auto pressed=pressedPhysical();
    if(bootPage==BootPage::Controls){
        if(pressed.empty()){calibrationReleased=true;return;}
        if(!calibrationReleased)return;calibrationReleased=false;
        int code=pressed.front();
        // The function/menu key can be reserved by Knulli. START skips only
        // this optional last binding; L2+SELECT will still open the home menu.
        if(calibrationIndex==MENU&&code==physicalMap[START])code=-1;
        else if(std::find(physicalMap.begin(),physicalMap.begin()+calibrationIndex,code)!=physicalMap.begin()+calibrationIndex){bootMessage="ALREADY USED - TRY AGAIN";return;}
        physicalMap[calibrationIndex++]=code;bootMessage.clear();
        if(calibrationIndex==BUTTON_COUNT){writeControls();physicalHeld.fill(false);for(int i=0;i<BUTTON_COUNT;i++)physicalHeld[i]=std::find(pressed.begin(),pressed.end(),physicalMap[i])!=pressed.end();bootPage=BootPage::Home;homeChoice=0;}
        return;
    }
    std::array<bool,BUTTON_COUNT> next{};for(int i=0;i<BUTTON_COUNT;i++)next[i]=physicalMap[i]>=0&&std::find(pressed.begin(),pressed.end(),physicalMap[i])!=pressed.end();
    // Publish L2 before A even if both become pressed in the same frame.
    if(next[L2]!=physicalHeld[L2])input(L2,next[L2]);
    for(int i=0;i<BUTTON_COUNT;i++)if(i!=L2&&next[i]!=physicalHeld[i])input(Button(i),next[i]);
    Uint32 now=SDL_GetTicks();for(int i=0;i<4;i++)if(directionRepeat[i].tick(next[i],now))input(Button(i),true);
    physicalHeld=next;
}
fs::path projectPath(int slot){return dataRoot/"projects"/("Pocket-"+num(slot)+".trackermac");}
void resetProject(){
    tm_play(0);for(auto &[id,p]:project.patterns)for(int t=0;t<tracks;t++)for(int s=0;s<steps;s++){tm_cell_at4(id,t,s,0,-1,100,100,0,0,0,0,0,0,0);tm_step_eq(id,t,s,0,0,0,0);}
    project=Project{};running=songPlay=rec=shift=detail=dirty=false;track=row=field=scroll=focus=songRow=0;undo.clear();clipboard=Cell{};setPage(PATTERN);publishAll();
}
void enterEditor(){activeProject=true;bootPage=BootPage::Editor;bootMessage.clear();running=songPlay=rec=shift=false;undo.clear();clipboard=Cell{};track=row=field=scroll=focus=songRow=0;setPage(PATTERN);}
void openHome(){if(modal==6)closeBrowser();tm_preview_stop();tm_play(0);running=false;shift=false;modal=0;homeChoice=activeProject?3:0;bootMessage.clear();bootPage=BootPage::Home;}
void runHomeChoice(int choice){
    if(choice==0){resetProject();enterEditor();}
    else if(choice==1){bootPage=BootPage::Load;loadChoice=1;bootMessage.clear();}
    else if(choice==2){resetProject();demo();enterEditor();}
    else if(choice==3&&activeProject){bootPage=BootPage::Editor;}
    else if(choice==4&&activeProject){quit=true;}
    else if(choice==3&&!activeProject){quit=true;}
    if(choice==5&&activeProject&&physicalPad)beginCalibration();
    if(choice==4&&!activeProject&&physicalPad)beginCalibration();
}
bool startupInput(Button b,bool down){
    if(bootPage==BootPage::Editor){if(down&&(b==MENU||(b==SELECT&&shift))){openHome();return true;}return false;}
    if(!down||bootPage==BootPage::Splash||bootPage==BootPage::Controls)return true;
    if(bootPage==BootPage::Home){int count=activeProject?5:4;if(physicalPad)count++;
        if(b==UP)homeChoice=(homeChoice+count-1)%count;if(b==DOWN)homeChoice=(homeChoice+1)%count;
        if(b==B&&activeProject)bootPage=BootPage::Editor;
        if(b==A){if(dirty&&(homeChoice==0||homeChoice==1||homeChoice==2||homeChoice==4)){pendingChoice=homeChoice;bootPage=BootPage::Confirm;}else runHomeChoice(homeChoice);}
    }else if(bootPage==BootPage::Load){if(b==UP)loadChoice=std::max(1,loadChoice-1);if(b==DOWN)loadChoice=std::min(8,loadChoice+1);if(b==B)openHome();if(b==A){std::string err;if(load(projectPath(loadChoice),err)){saveSlot=loadChoice;dirty=false;enterEditor();}else bootMessage=err.empty()?"LOAD FAILED":err;}}
    else if(bootPage==BootPage::Confirm){if(b==B)openHome();if(b==A)runHomeChoice(pendingChoice);}
    return true;
}
void drawStartup(){
    if(bootPage==BootPage::Splash){Uint32 elapsed=SDL_GetTicks()-bootAt;
        for(int c=0;c<8;c++){int head=(int(elapsed/35)+c*5)%30;for(int r=0;r<16;r++){int y=((head-r+30)%30)*18-40;if(y<0||y>=480)continue;unsigned v=unsigned(c*93+r*17+head*13);auto n=noteName(36+v%48);text(c*80+3,y,n+"  "+num(v%48),r<3?cyan:r<8?green:panel,1);}}
        if(elapsed>1250){box(30,195,580,90,black);center(0,220,640,"TERMIN TRACKER",white,4);center(0,260,640,"POCKET",green,2);}
        if(elapsed>=2000){if(handheld&&physicalPad&&!readControls())beginCalibration();else bootPage=BootPage::Home;}
    }else if(bootPage==BootPage::Controls){center(0,42,640,"CONTROLLER SETUP",white,3);center(0,105,640,"ONE TIME PHYSICAL BUTTON MAPPING",olive,1);center(0,178,640,"PRESS AND RELEASE",cyan,2);center(0,225,640,physicalNames[calibrationIndex],white,3);center(0,285,640,num(calibrationIndex+1)+" / "+num(BUTTON_COUNT),green);center(0,335,640,bootMessage.empty()?"USE THE LABEL ON YOUR DEVICE":bootMessage,olive,1);if(calibrationIndex==MENU)center(0,365,640,"START TO SKIP IF MENU IS RESERVED",cyan,1);
    }else if(bootPage==BootPage::Home){center(0,72,640,"TERMIN TRACKER",white,4);center(0,116,640,"POCKET",green,2);std::vector<std::string> choices={"NEW","LOAD","DEMO"};if(activeProject)choices.push_back("CONTINUE");choices.push_back("EXIT");if(physicalPad)choices.push_back("CONTROLLER SETUP");for(int i=0;i<(int)choices.size();i++){int y=164+i*40;box(130,y,380,34,i==homeChoice?selection:panel);if(i==homeChoice)border(130,y,380,34,green);center(130,y+8,380,choices[i],i==homeChoice?white:olive);}
        center(0,438,640,"D-PAD SELECT  A CONFIRM",cyan,1);
    }else if(bootPage==BootPage::Load){center(0,38,640,"LOAD PROJECT",white,3);for(int i=1;i<=8;i++){int y=93+(i-1)*35;box(94,y,452,30,i==loadChoice?selection:panel);text(108,y+7,"SLOT "+num(i));text(350,y+7,fs::exists(projectPath(i))?"SAVED":"EMPTY",olive);}center(0,405,640,bootMessage.empty()?"A LOAD  B BACK":bootMessage,cyan,1);
    }else if(bootPage==BootPage::Confirm){center(0,180,640,"UNSAVED PROJECT",white,3);center(0,242,640,"A DISCARD  B CANCEL",cyan);}
    SDL_RenderPresent(renderer);
}
