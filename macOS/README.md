# Termin Tracker 0.19.1 — icon & DMG

0.19.1 dağıtımı, kullanıcının orijinal 1024×1024 TerminIcon.png dosyasından oluşturulan çok çözünürlüklü macOS ikonunu ve sıkıştırılmış DMG kurulum imajını içerir. Hazır uygulama Apple Silicon + Intel universal binary'dir (macOS 15 veya üstü); Intel binary çapraz derlenmiştir, çalışma testi Apple Silicon üzerinde yapılmıştır. Uygulama yerel ad-hoc imzalıdır, Apple notarization yoktur. Kurulum ayrıntıları KURULUM.txt dosyasında. Build.command kendi Mac mimariniz için yeniden derler ve ikonu korur.

TrackerMac'in yeni adı **Termin Tracker**. 0.18 üzerine yapılan bu 0.19 güncellemesinde ses motoru, keyboard mode, song follow ve proje formatı korunmuştur. Hazır Apple Silicon macOS uygulaması `TerminTracker-0.19.app` dosyasındadır. Çift tıklayarak açabilirsiniz. Eski .trackermac projeleri yüklenebilir. 0.15 için hazırlanan 40 slaytlık kılavuz eski ekranları anlatır; güncel özellikler aşağıdadır.

## Yeni görsel kimlik ve açılış

- Siyah #000000; panel #131313; ölçü vurgusu #222121; çizgiler #515151.
- Nota #C0C0C0, instrument/velocity/volume #6C7448, pan/time/FX2 #546D54, FX1 #598E8E. Diğer durum renkleri mat ve düşük doygunluklu.
- Açılışta iki saniye boyunca rastlantısal tracker satırları yukarıdan aşağı akar, sonra kaybolur ve TERMIN TRACKER ana menüsü belirir. Animasyon hiçbir ses üretmez.
- Ana menüde Start Audio ile Audio Setup açılır. İstersen New, Load veya Demo'yu doğrudan seç: audio setup tamamlanınca seçtiğin işlem devam eder. Menüye Esc ile dönebilirsin. MIDI ve kayıt izni akışı korunur.

## Uzun nota (harici MIDI olmadan)

Pattern ekranında nota veya akorun başladığı satırı seçin (devam satırı da olur). Normal modda `K` ile Note Duration penceresini açın. `E` / `Q` 0.25 step ekler/çıkarır. Shift + E/Q tam 1 step değiştirir. Enter ile kaydedin. Örneğin 01. satırdaki F2'yi 4 step tutmak için 4.00 steps seçin; 02–04. satırlarda aynı nota %60 opaklıkla gösterilir. `L` ayrı bir note-off ekler. One-shot sample fiziksel süresinden uzun çalamaz; loop, granular ve Omondeu için uzun gate daha belirgindir.

## Computer Keyboard

- Pattern'de instrument atanmış track'i seçin. `Cmd+K` ile Keyboard Mode açılır/kapanır. Sağ alt köşedeki piyano basılı tuşları, nota adlarını ve oktavı gösterir.
- A C, W C#, S D, E D#, D E, F F, T F#, G G, Y G#, H A, U A#, J B, K sonraki C. Z alt oktav, X üst oktav. Tuş basılı kaldıkça nota çalar; bırakınca note-off gönderilir. Auto-repeat yeni nota üretmez.
- REC kapalıyken serbestçe çalın. REC açık ve transport durmuşken akor / tek nota step olarak girilir. Akor tuşlarını yakın zamanlı basın (7 Config > MIDI chord window; varsayılan 65 ms).
- Gerçek zamanlı süre kaydı: transport durmuşken R'yi basılı tutup Space'e basın, sonra R'yi bırakın. Notayı veya akoru fiziksel tuşlarla istediğiniz süre basılı tutun: normal renk başlangıç + soluk devam satırları. Space durdurur, Cmd+K kapanır. K ancak Keyboard Mode kapalıyken süre düzenleyicidir.
- E, F, G, H, U, X gibi çakışan tuşlar Keyboard Mode'da müzik tuşlarıdır. R, Space, oklar ve numara ile sayfa geçişleri çalışır; açık bir düzenleme penceresinde pencerenin kontrolleri önceliklidir. Harici MIDI ayarları korunur. Uygulama odağı kaybolduğunda basılı bilgisayar notaları bırakılır.

## Song Follow

Song sayfasında Space ile song chain'i başlatın, ardından 1 ile Pattern'e dönün. Pattern numarası, pattern uzunluğu, track atamaları, isimleri ve yeşil playhead gerçek çalan pattern'e göre otomatik güncellenir. Açık düzenleme penceresi veya Fill preview sırasında yanlış pattern'e yazmamak için geçiş bekletilir; pencereyi kapatınca güncel pattern görülür. Playback durunca son çalan pattern seçili kalır.

## Audio Setup — ilk açılış

1. Açılış menüsünde Start Audio'yu seçince macOS varsayılan Output, Record Input, bağlı MIDI kaynakları, sample rate ve mikrofon izin durumu gösterilir. Cihazları macOS System Settings > Sound üzerinden seçin; R ile listeyi yenileyin.
2. Enter ile **Start Audio + Enable Live Record** seçin. Gerekliyse macOS mikrofon izni sorar. İzinle birlikte ses motoru başlar ve New / Load / Demo menüsü açılır. Bu ekranda hiçbir ses kaydedilmez.
3. İzin vermek istemiyorsanız Start Audio / Playback Only seçin. Sequencer, synth, sampler ve MIDI normal çalışır. Önceden verilmiş izin geri alınmaz.
4. Live Record'da artık izin penceresi açılmaz. İzin yoksa kayıt ekranı Audio Setup'a yönlendirir. H → Audio Setup ile izin durumunu kontrol edebilirsiniz. Önceden reddedilmiş izin için System Settings → Privacy & Security → Microphone'da Termin Tracker'ı etkinleştirin.
5. Kayıt her zamanki gibi 8 → F4 Live Record → F1 Record. Yalnızca F1/R/Space ile başlatınca giriş motoru açılır; Stop ile kapanır.

## Omondeu — hızlı başlangıç

1. Pattern sayfasında istediğiniz track'i seçin. `3` → Instruments sayfasında **10 Omondeu** seçin, Enter ile track'e atayın. Alternatif: Pattern'de `I` → Enter → instrument seçin.
2. Pattern veya Instruments sayfasında `U` ile synth ekranını açın. Instrument sayfasında F7 de çalışır.
3. Oklarla parametre seçin, Q/E ile değiştirin, Shift ile hızlandırın. F1 veya Enter seçili oktavda sesi yaklaşık iki saniye dinletir; [ / ] oktavı ayarlar. MIDI klavye ve Option + pad ile de ses duyabilirsiniz.
4. F2/F3 altı hazır ses arasında gezinir: Deep Analog, Pure Sub, Acid Pulse, Wide Reese, Velvet Keys, Slow Motion. Preset değiştirmek mevcut patch'i değiştirir; Cmd+Z ile geri alabilirsiniz. F4/F5/F6/F7 ilgili parametre grubuna gider; Esc veya F8 geri döner.
5. Birden fazla Omondeu için Instruments sayfasında boş slot seçip F2/F3 ile Omondeu wave'ine geçin. Her slot bağımsız patch saklar. Proje kaydı parametreleri de içerir; song export aynı synth motorunu kullanır.

### Bass tasarımı

- Deep Analog: C2–G2 civarında başlayın. Sub Octave -1, Sub Mix 75%, Cutoff 800 Hz; cutoff'u azaltınca daha yumuşak, Drive ve Resonance yükselince daha agresif bir karakter oluşur.
- Pure Sub: Sine ve merkezdeki sub. Legato ile üst üste tutulan notalar arasında yumuşak glide. 55 Hz için A2 ve sub octave -1.
- Acid Pulse: Pulse ve saw, yüksek resonance ve kısa filter decay. Pulse Width ile karakteri değiştirin.
- Wide Reese: Saw, detune ve %100 Stereo Width. Sine sub merkezde, detuned katmanın stereo farkı 120 Hz civarında high-pass'ten geçer; mono uyumu güçlenir.
- Velvet Keys / Slow Motion: Poly (en çok 8 notalık chord), envelope ve LFO ile pad ve lead.

Wave A/B: Sine, Saw, Pulse, Triangle. Osc A/B/Sub Mix bağımsız; B Semitone ±24, Detune 0–40 cent, Sub Octave -2…0. Low-pass Cutoff 30–16000 Hz, Resonance, Filter Env, Filter Decay, Drive. Amp ADSR, Mono/Poly/Legato, Glide, Stereo Width, Pulse Width ve cutoff LFO (0–10 Hz).

Poly/Mono/Legato sequencer ve MIDI'de çalışır. Legato üst üste gelen notalarda envelope'u yeniden başlatmadan pitch'i kaydırır; tracker'da `K` ile nota süresi ve `L` ile note-off kullanın. Süre girilmemiş notalar bir step gate ile çalar. Omondeu bağımlılık gerektirmeyen, özgün C++ DSP'dir; donanım devresi emülasyonu değildir. 24×8 voice gibi ekstrem yükler için Config (7) DSP ölçümlerini izleyin.

## Sampler: octave ve Time Stretch

Sample Playback (`9`) veya Sample Editor (`0`) içinde `O` seçip Q/E ile -3…+3 oktav değiştirin. Bu enstrümana özel playback ayarıdır; orijinal PCM değişmez. Root nota ayrı ayardır. F1 ile önizleyin.

Time Stretch: `0` Sample Editor > Start/End ile bölge seçin > F7/F8 ile Time Stretch > `B` > Q/E (%50–200, Shift ile 10 puan) > F2 Apply/Enter. Sonucu F1 ile dinleyin. F3 Undo eski PCM'yi geri getirir. Yalnız seçilen bölge değişir; kalan waveform ve slice işaretleri korunur/ölçeklenir. Rubber Band R3 offline motoru (Mac'inizde kurulu 4.0.0) kullanılır. Bu işlem audio callback'te çalışmaz. Başka bir Mac'te yalnızca Time Stretch için `brew install rubberband` gerekebilir. Dış aracı uygulama paketine dahil etmedik. 100% no-op, 50% yarı süre, 200% iki kat süre. Çok kısa veya karmaşık seslerde/ekstrem oranlarda artefakt olabilir. Slot başına 1 milyon frame, proje PCM 64 MB sınırı korunur.

## Compressor ve sidechain

`X` FX Settings > F6 Dynamics (veya `C`). Source Track T01–T24. Yeni projelerde Track 1 compressor ve master ducking varsayılan açık; daha önceki projelerin mix'i değişmesin diye eski kayıtlarda devre dışı. İstersen F5 Reset ile yeni varsayılanı etkinleştir. Compressor threshold -24 dB, ratio 3:1, makeup 0 dB; sidechain threshold -30 dB, maksimum duck 6 dB. Her ikisi için attack 5 ms, release 150 ms. Q/E ve oklarla ayarlayın; iki efekt ayrı ON/OFF. Source kuru sinyali ducking dışında kalır, diğer kanallar ve efekt dönüşleri azalır. Gain reduction metreleri gerçek DSP gain düşüşünü gösterir; doğrudan MIDI audition, pattern channel dynamics zincirine dahil olmaz. Master sidechain ancak source kanalı playback'te ses ürettiğinde çalışır. Kaydetme ve export'a dahil.

## Mac'te derleme

1. ZIP'i açın. Mac'te Xcode Command Line Tools kurulu değilse Terminal'de `xcode-select --install` çalıştırın.
2. `Build.command` dosyasını Terminal'de `bash ` yazıp dosyayı pencereye sürükleyerek çalıştırın. İsterseniz doğrudan çift tıklayın.
3. Başarılı derleme, imza ve self-test işleminden sonra aynı dizinde `TerminTracker-0.19.app` oluşur.

macOS 15.7.3 üzerinde arm64 derleme, kod imzası, workflow ve WAV export self-testleri başarılıdır. C++ DSP ve önceki motor regresyon testleri Linux üzerinde başarılıdır. Fiziksel MIDI klavye ile gerçek kullanım testi henüz yapılmadı. Mac'te test başarısız olursa derleme scripti durur. Paketteki 0.9 sunumu eski kullanım kılavuzudur; yeni kısayollar aşağıdadır.

## EQ

`X` → `F7 EQ Graph` (veya `A`): EQ. Sol/sağ bant, Q/E kazanç, Shift ile 3 dB, Delete bant sıfırlama, F5 tümünü sıfırlama. A/Tab ile Reverb/Delay sayfasına dönün. Bass 100 Hz, Mid A 500 Hz, Mid B 2.5 kHz, High 8 kHz; ±18 dB. Grafik gerçek filtre tepkisini gösterir. EQ master çıkışta, peak protection öncesindedir.

**Step EQ:** Pattern > R (REC) > F/G ile FX1/FX2 > Enter > sol/sağ ile EQ > Enter. Grafik açılır; dört bandı Q/E ile ayarlayın, Enter ile kaydedin, Esc ile iptal. Hücrede EQ görünür. EQ yalnız ilgili step süresince uygulanır; uzun nota devam ediyorsa yeni nota tetiklemeden filtrelenebilir. Her iki FX slotunda da EQ seçilmişse bant kazançları toplanır ve ±18 dB ile sınırlandırılır. Sonraki step EQ içermiyorsa step filtresi düzleşir. 

**Channel EQ:** 4 (Mixer) > track seç > F3/W (FX Sends) > EQ / 4-BAND > Enter. İlgili pattern ve track için bağımsız grafik EQ. Master EQ, bu ayardan etkilenmez. EQ, send ile aynı menüden erişilse de bir insert filtredir; reverb/delay'e giden sinyali de şekillendirir. Master Output EQ seçilirse global master EQ düzenlenir.

**Fill:** FX Amount, EQ/reverb/delay/slicer yoğunluğunu yönetir. Random özellikle EQ kazançlarını ve efekt seçimlerini değiştirir; iki FX slotu öncelikli gate/retrigger komutları korunarak doldurulur. FX Amount 0 yeni efekt eklemez.

## Uzun nota ve live MIDI

Pattern'de R+Space ile canlı kayıt başlatın; seçili track'e instrument atayın ve USB MIDI klavyenizde çalın. Note-on ve note-off zamanlarından süre hesaplanır; nota başlangıcında tam renk, devam satırlarında %60 opacity. Devam satırları ayrı tetikleme değildir. Grup olarak kaydedilen akor bir ortak süre taşır; aynı track'te yeni tetikleme eski notayı keser. Pattern sınırında süre sonlanır. One-shot sample doğal sonunda biter; loop, granular ve synth süre boyunca devam edebilir.

Manuel düzenleme için nota ya da devam satırında K. Q/E ile çeyrek step, Shift+Q/E ile bir step, Enter uygula. Delete legacy/natural süreye döndürür.

## Fill

Pattern > REC > F4 Fill. Parameters içinde Sustain (varsayılan %65) uzun melodik nota olasılığını ayarlar. Bass, Lead ve Chords için sonraki tetiklemeye kadar farklı süreler oluşturur. Davul preset'leri süre davranışını korur. %0 eski davranıştır.

## Mixer FX sends

4 (Master/Mixer) ekranında bir track veya Master Output seçip F3 FX Sends veya W basın (ya da R/D/S değerlerinin olduğu satıra tıklayın). Up/Down efekt, Q/E miktar, Shift+Q/E %10, Delete sıfır, Enter/Esc kapat. Reverb, Delay, Slicer için %0–100 arası ayrı kontrol vardır. Track send'leri pattern bazlı, Master send globaldir. Slicer dry/wet ritmik kesme uygular; reverb/delay ortak efekt dönüşlerini kullanır. Master send efekt dönüşlerini tekrar beslemez. Bunlar pattern/song playback ve WAV export için geçerlidir; doğrudan MIDI audition kanal send'lerine henüz dahil değildir.

Süre, EQ, send ve Fill ayarları projeyle birlikte saklanır; mevcut 0.12 projeleri yeni alanlar yokken varsayılan değerlerle açılır.
