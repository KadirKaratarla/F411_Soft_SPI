# F411_Soft_SPI

🔧 STM32F411 ile Yazılımsal ve Donanımsal SPI Haberleşme Uygulaması

STM32F411 "Black Pill" kartı üzerinde hem yazılımsal (bit-banging) hem de donanımsal SPI (SPI1/SPI2) haberleşmesini destekleyen modüler bir sürücü geliştirdim.

Bu projede amaç, SPI protokolünün düşük seviyede hem GPIO kontrollü (software) hem de native SPI periferleri kullanılarak (hardware) nasıl uygulanabileceğini göstermekti.

💡 Neden SPI ve Neden Software SPI?
🔹 SPI, UART ve I2C'ye göre daha yüksek veri hızlarına izin verir ve yapısı itibariyle oldukça basittir (senkron, full-duplex, master-slave).
🔹 Ancak bazı uygulamalarda, donanımsal SPI pinleri yetersiz kalabilir veya SPI çevresel birimi kullanılamaz hâle gelebilir.
🔹 Bu gibi durumlarda GPIO’lar üzerinden SPI sinyallerini yazılım kontrollü olarak üretmek mümkündür ve bu yöntem software SPI (SSPI) olarak bilinir.

🔨 Uygulama Adımları (SSPI ve HSPI)
1. Software SPI (SSPI):
CPOL ve CPHA değerlerini dinamik yapılandırılabilir hale getirerek SPI mod 0–3 desteği sağladım.

SPI işlemini:

MOSI hattına bit yerleştirme

Saat (SCK) darbesi üretimi

MISO hattından veri okuma
adımlarıyla bit bazlı olarak yönettim.

GPIO seviyesinde işlem yapıldığı için düşük frekansta çalışmak avantajlıdır.

2. Hardware SPI (HSPI):
SPI1 ve SPI2 çevresel birimlerini yapılandırmak için:

Gerekli clock aktiflemeleri

Pinleri alternate function mode'a alma

SPI_InitTypeDef yapısını doldurarak:

Baudrate

CPOL/CPHA

Master/Slave

MSB/LSB
gibi parametreleri tanımladım.

SPI donanımı aktifleştirilerek tam verimli SPI haberleşme sağlandı.

✅ Kod Özellikleri:
SPI modları (Mode 0-3) desteklenmektedir.

SSPI_Data() ve HSPI_Data() fonksiyonları ile veri alışverişi yapılabilir.

SSPI_Recv() ve HSPI_Recv() makroları ile yalnızca okuma yapılabilir.

Genişletilebilir yapı: SPI3 veya diğer protokollere geçişe uygun.

📦 Geliştirme Ortamı:
MCU: STM32F411CEU6 (Black Pill)

Editör: IAR Embedded workbench 
