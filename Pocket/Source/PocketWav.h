// RIFF/WAVE decoder for PCM 8/16/24/32-bit and IEEE float 32/64-bit.
// Keeps the source sample rate: Termin's resampler handles pitched playback.
static uint16_t wav16(const unsigned char* p){return uint16_t(p[0])|(uint16_t(p[1])<<8);}
static uint32_t wav32(const unsigned char* p){return uint32_t(p[0])|(uint32_t(p[1])<<8)|(uint32_t(p[2])<<16)|(uint32_t(p[3])<<24);}
bool decodeWav(const fs::path& path,Slot& result,std::string& error){
    std::error_code ec;auto size=fs::file_size(path,ec);
    if(ec||size<44||size>32000000){error="WAV MISSING OR TOO LARGE";return false;}
    std::ifstream f(path,std::ios::binary);std::vector<unsigned char> b(size);
    if(!f.read(reinterpret_cast<char*>(b.data()),b.size())||std::memcmp(b.data(),"RIFF",4)||std::memcmp(b.data()+8,"WAVE",4)){error="NOT A RIFF WAV FILE";return false;}
    const unsigned char *fmt=nullptr,*pcm=nullptr;size_t fmtSize=0,pcmSize=0;
    for(size_t at=12;at+8<=b.size();){size_t n=wav32(b.data()+at+4),start=at+8;if(n>b.size()-start){error="TRUNCATED WAV";return false;}if(!std::memcmp(b.data()+at,"fmt ",4)){fmt=b.data()+start;fmtSize=n;}else if(!std::memcmp(b.data()+at,"data",4)){pcm=b.data()+start;pcmSize=n;}at=start+n+(n&1);}
    if(!fmt||fmtSize<16||!pcm){error="WAV CHUNKS MISSING";return false;}
    int format=wav16(fmt),channels=wav16(fmt+2),bits=wav16(fmt+14),block=wav16(fmt+12);uint32_t sampleRate=wav32(fmt+4);
    if(format==0xfffe&&fmtSize>=40)format=wav16(fmt+24);
    if(channels<1||channels>8||sampleRate<8000||sampleRate>192000||block<channels*((bits+7)/8)||!block||!((format==1&&(bits==8||bits==16||bits==24||bits==32))||(format==3&&(bits==32||bits==64)))){error="UNSUPPORTED WAV FORMAT";return false;}
    size_t n=pcmSize/block;if(n<1||n>1000000){error="MAX 1M SAMPLE FRAMES";return false;}
    Slot s;s.name=path.stem().string();s.wave=2;s.sampleRate=sampleRate;s.pcm.resize(n);s.end=int(n);
    auto folder=path.parent_path().filename().string();if(folder.find("Kick")!=std::string::npos)s.root=36;else if(folder.find("Snare")!=std::string::npos)s.root=50;else if(folder.find("HiHat")!=std::string::npos)s.root=66;
    // Factory pitched samples use filenames ending in e.g. _C4_v100.
    for(size_t i=0;i+3<s.name.size();i++)if(s.name[i]=='_'){
        std::string names="C D EF G A B";const char ch=s.name[i+1];auto p=names.find(ch);size_t j=i+2;int pitch=int(p);
        if(p==std::string::npos||ch==' ')continue;if(s.name[j]=='#'){pitch++;j++;}else if(s.name[j]=='b'){pitch--;j++;}
        if(j<s.name.size()&&s.name[j]>='0'&&s.name[j]<='8'&&j+1<s.name.size()&&s.name[j+1]=='_'){s.root=clamp((s.name[j]-'0'+1)*12+pitch,0,127);break;}
    }
    const int bytes=(bits+7)/8;
    for(size_t i=0;i<n;i++){double sum=0;for(int c=0;c<channels;c++){const auto* p=pcm+i*block+c*bytes;double v=0;
        if(format==3){if(bits==32){uint32_t v32=wav32(p);float a;std::memcpy(&a,&v32,4);v=a;}else{uint64_t u=uint64_t(wav32(p))|(uint64_t(wav32(p+4))<<32);std::memcpy(&v,&u,8);}}
        else if(bits==8)v=(int(p[0])-128)/128.;else if(bits==16)v=int16_t(wav16(p))/32768.;else if(bits==24){int32_t q=int32_t(p[0])|(int32_t(p[1])<<8)|(int32_t(p[2])<<16);if(q&0x800000)q-=0x1000000;v=q/8388608.;}else v=int32_t(wav32(p))/2147483648.;
        if(std::isfinite(v))sum+=std::clamp(v,-1.,1.);
    }s.pcm[i]=float(sum/channels);}
    result=std::move(s);error.clear();return true;
}
