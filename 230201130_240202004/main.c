#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
#include <curl/curl.h>
#define Rectangle RaylibRectangle
#define CloseWindow CloseWindow1
#define ShowCursor ShowCursor1
#define DrawTextEx DrawTextEx1
#define PlaySound PlaySound1
#define LoadImage LoadImage1
#define NOGDI // GDI fonksiyonlarını devre dışı bırak
#undef DrawText
#include "raylib.h"
#include <windows.h> // Windows API için
#undef CloseWindow // Makroyu geçersiz kı
// Windows'un DrawText makrosunu kaldır
#define GRID_SIZE 20 // Izgara boyutu (20x20)
#define CELL_SIZE 50 // Her hücrenin boyutu (40x40)
#define MAX_UNITS_PER_CELL 100 // Her hücrede maksimum 100 birim
#define MAX_LINE_LENGTH 512
#define MAX_CREATURES 100
#define MAX_HEROES 100
#define MAX_RESEARCH 100
#define MAX_UNIT_TYPES 100
#define MAX_KAHRAMAN 10
#define MAX_CANAVAR 10
// Birim yapısı
typedef struct {
 int saldiri;
 int savunma;
 int saglik;
 int kritik_sans;
} Unit;
// İnsan İmparatorluğu yapısı
typedef struct {
 Unit piyadeler;
 Unit okcular;
 Unit suvariler;
 Unit kusatma_makineleri;
} UnitInsan;
// Ork Lejyonu yapısı
typedef struct {
 Unit ork_dovusculeri;
 Unit mizrakcilar;
 Unit varg_binicileri;
 Unit troller;
} UnitOrg;
typedef struct {
 int deger;
 char aciklama[256];
} Seviye;
typedef struct {
 Seviye seviye_1;
 Seviye seviye_2;
 Seviye seviye_3;
} SavunmaBasari;
typedef struct {
 SavunmaBasari savunma_ustaligi;
 SavunmaBasari saldiri_gelistirmesi;
 SavunmaBasari elit_egitim;
 SavunmaBasari kusatma_ustaligi;
} ArastirmaBasarilari;
// İnsan birimleri yapısı
typedef struct {
 int piyadeler;
 int okcular;
 int suvariler;
 int kusatma_makineleri;
} InsanBirimleri;
// Ork birimleri yapısı
typedef struct {
 int ork_dovusculeri;
 int mizrakcilar;
 int varg_binicileri;
 int troller;
} OrkBirimleri;
// Araştırma seviyesi yapısı
typedef struct {
 int savunma_ustaligi;
 int saldiri_gelistirmesi;
 int kusatma_ustaligi;
 int elit_egitim;
} ArastirmaSeviyesi;
typedef struct {
 int etki_degeri;
 char etki_turu[50];
 char aciklama[256];
} Yaratik;
typedef struct {
 Yaratik ejderha;
 Yaratik agri_dagi_devleri;
 Yaratik tepegoz;
 Yaratik karakurt;
 Yaratik samur;
} YaratikInsan;
typedef struct {
 Yaratik kara_troll;
 Yaratik golge_kurtlari;
 Yaratik camur_devleri;
 Yaratik ates_iblisi;
 Yaratik buz_devleri;
 Yaratik makrog_savas_beyi;
} YaratikOrg;
// Kahraman yapısı
typedef struct {
 char ad[30];
 char bonus_turu[30];
 int bonus_degeri;
 char aciklama[256];
} Kahraman;
// İnsan İmparatorluğu kahramanları yapısı
typedef struct {
 Kahraman kahramanlar[MAX_KAHRAMAN];
 int kahraman_sayisi;
} InsanKahramanlari;
// Ork Lejyonu kahramanları yapısı
typedef struct {
 Kahraman kahramanlar[MAX_KAHRAMAN];
 int kahraman_sayisi;
} OrkKahramanlari;
// İmparatorluk yapısı
typedef struct {
 InsanBirimleri insan_birimleri;
 OrkBirimleri ork_birimleri;
 UnitInsan unit_insan;
 UnitOrg unit_org;
 char kahramanlar[MAX_KAHRAMAN][30];
 char canavarlar[MAX_CANAVAR][30];
 ArastirmaSeviyesi arastirma_seviyesi;
 ArastirmaBasarilari arastirma_basarilari;
 YaratikInsan yaratik_insan;
 YaratikOrg yaratik_org;
 Kahraman insan_kahramanlar[MAX_KAHRAMAN];
 Kahraman ork_kahramanlar[MAX_KAHRAMAN];
 int insan_kahraman_sayisi;
 int ork_kahraman_sayisi;
} Imparatorluk;
void BirimleriYerlestir(Texture2D texture, int toplamBirim, int saglik, int sutun, int baslangicY) {
    int fullSquares = toplamBirim / 100;  // Gerekli tam kareleri hesapla
    int remainingUnits = toplamBirim % 100; // Kalan birimler


    Color renk ;
    if (saglik > 70.0f) {
    renk = GREEN;
} else if (saglik > 20.0f) {
    renk = YELLOW;
} else {
    renk = RED;
}  // %30'dan düşükse kırmızı, aksi halde yeşil

    // Tüm kareler için
    for (int i = 0; i < fullSquares; i++) {
        int y = baslangicY + i; // Her tam kare için Y koordinatı

        if (y < GRID_SIZE) {
            // Siyah bir çerçeve çiz
            DrawRectangleLines((sutun + 1) * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK); // Siyah çerçeve
            DrawRectangleLines(sutun * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK); // Görselin olduğu hücreye siyah çerçeve

            // Sağlık durumuna göre yeşil veya kırmızı kutu çiz
            DrawRectangle((sutun + 1) * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, renk); // Sağlık durumu kutusu

            // Görseli çiz
            DrawTexture(texture, sutun * CELL_SIZE, y * CELL_SIZE, WHITE); // Görseli çiz

            // Kutu üzerine birim sayısını yaz (örneğin: 100)
            DrawText("100", (sutun + 1) * CELL_SIZE + 10, y * CELL_SIZE + 10, 20, BLACK); // 100 birim yazısı
        }
    }

    // Kalan birimleri çiz
    if (remainingUnits > 0) {
        int y = baslangicY + fullSquares; // Kalan birimlerin Y koordinatı
        if (y < GRID_SIZE) {
            // Siyah bir çerçeve çiz
            DrawRectangleLines((sutun + 1) * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK); // Siyah çerçeve
            DrawRectangleLines(sutun * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK); // Görselin olduğu hücreye siyah çerçeve

            // Sağlık durumuna göre renkli kutu çiz
            DrawRectangle((sutun + 1) * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, renk); // Kırmızı veya yeşil kutu

            // Görseli çiz
            DrawTexture(texture, sutun * CELL_SIZE, y * CELL_SIZE, WHITE); // Görseli çiz

            // Kalan birim sayısını kutu üstüne yaz
            char kalanBirimText[10];
            sprintf(kalanBirimText, "%d", remainingUnits); // Kalan birimi stringe çevir
            DrawText(kalanBirimText, (sutun + 1) * CELL_SIZE + 10, y * CELL_SIZE + 10, 20, BLACK); // Kalan birim sayısını yaz
        }
    }

    // Tüm kutuların etrafında siyah çerçeve
    DrawRectangleLines(sutun * CELL_SIZE, baslangicY * CELL_SIZE, CELL_SIZE, (fullSquares + (remainingUnits > 0 ? 1 : 0)) * CELL_SIZE, BLACK); // Siyah çerçeve
}
// Dosyayı okuyan ve verileri struct'a yerleştiren fonksiyon
void dosyadan_verileri_yukle_senaryo(Imparatorluk *imparatorluk, const char *dosya_adi) {
    FILE *file = fopen(dosya_adi, "r");
    if (!file) {
            printf("Dosya açılamadı!\n");
    return;
 }
 char satir[256];
 while (fgets(satir, sizeof(satir), file)) {
 // İnsan İmparatorluğu birimleri
 if (strstr(satir, "\"piyadeler\"")) {
        sscanf(satir, " \"piyadeler\": %d,", &imparatorluk->insan_birimleri.piyadeler);
 } else if (strstr(satir, "\"okcular\"")) {
     sscanf(satir, " \"okcular\": %d,", &imparatorluk->insan_birimleri.okcular);
     } else if (strstr(satir, "\"suvariler\"")) {
         sscanf(satir, " \"suvariler\": %d,", &imparatorluk->insan_birimleri.suvariler);
         } else if (strstr(satir, "\"kusatma_makineleri\"")) {
             sscanf(satir, " \"kusatma_makineleri\": %d,", &imparatorluk->insan_birimleri.kusatma_makineleri);
             }
 // Ork Lejyonu birimleri
 else if (strstr(satir, "\"ork_dovusculeri\"")) {
        sscanf(satir, " \"ork_dovusculeri\": %d,", &imparatorluk->ork_birimleri.ork_dovusculeri);
 } else if (strstr(satir, "\"mizrakcilar\"")) {
     sscanf(satir, " \"mizrakcilar\": %d,", &imparatorluk->ork_birimleri.mizrakcilar);
     } else if (strstr(satir, "\"varg_binicileri\"")) {
         sscanf(satir, " \"varg_binicileri\": %d,", &imparatorluk->ork_birimleri.varg_binicileri);
         } else if (strstr(satir, "\"troller\"")) {
             sscanf(satir, " \"troller\": %d,", &imparatorluk->ork_birimleri.troller);
             }
}
fclose(file);
}
void extract_data(const char *line, const char *key, char *output) {
    char *start = strstr(line, key);
    if (start) {
            start = strchr(start, '[') + 1; // '[' karakterinden sonraki ilk karakter
    char *end = strchr(start, ']'); // ']' karakterine kadar
    if (end) {
            size_t len = end - start;
    strncpy(output, start, len);
    output[len] = '\0';
    }
    }
}
void parse_json_senaryo(const char *filename, Imparatorluk *insan_imparatorlugu, Imparatorluk*ork_legi) {
    FILE *file = fopen(filename, "r");
    if (!file) {
            printf("Dosya açılamadı.\n");
    exit(1);
 }
 char line[256];
 char temp[256];
 int kahraman_index_insan = 0, canavar_index_insan = 0;
 int kahraman_index_ork = 0, canavar_index_ork = 0;
 int okuma_durumu = 0; // 0: hiçbir şey, 1: insan imparatorluğu, 2: ork legi
 while (fgets(line, sizeof(line), file)) {
 // İnsan İmparatorluğu bölümünü bulma
 if (strstr(line, "insan_imparatorlugu")) {
        okuma_durumu = 1; // İnsan bölümüne geçiş
 continue;
 }
 // Ork Legi bölümünü bulma
 else if (strstr(line, "ork_legi")) {
        okuma_durumu = 2; // Ork bölümüne geçiş
 continue;
 }
 // İnsan İmparatorluğu verilerini okuma
 if (okuma_durumu == 1) {
 if (strstr(line, "kahramanlar")) {
     extract_data(line, "\"kahramanlar\":", temp);
 char *token = strtok(temp, ",");
 while (token && kahraman_index_insan < MAX_KAHRAMAN) {
     sscanf(token, " \"%[^\"]\"", insan_imparatorlugu->kahramanlar[kahraman_index_insan++]);
 token = strtok(NULL, ",");
 }
 } else if (strstr(line, "canavarlar")) {
     extract_data(line, "\"canavarlar\":", temp);
 char *token = strtok(temp, ",");
 while (token && canavar_index_insan < MAX_CANAVAR) {
        sscanf(token, " \"%[^\"]\"", insan_imparatorlugu->canavarlar[canavar_index_insan++]);
 token = strtok(NULL, ",");
 }
 } else if (strstr(line, "arastirma_seviyesi")) {
     while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "\"savunma_ustaligi\"")) {
                sscanf(line, " \"savunma_ustaligi\": %d,", &insan_imparatorlugu->arastirma_seviyesi.savunma_ustaligi);
     } else if (strstr(line, "\"saldiri_gelistirmesi\"")) {
         sscanf(line, " \"saldiri_gelistirmesi\": %d,", &insan_imparatorlugu->arastirma_seviyesi.saldiri_gelistirmesi);
         } else if (strstr(line, "\"kusatma_ustaligi\"")) {
             sscanf(line, " \"kusatma_ustaligi\": %d,", &insan_imparatorlugu->arastirma_seviyesi.kusatma_ustaligi);
             } else if (strstr(line, "\"elit_egitim\"")) {
                 sscanf(line, " \"elit_egitim\": %d,", &insan_imparatorlugu->arastirma_seviyesi.elit_egitim);
                 }
 // Araştırma seviyeleri sona erdiğinde döngüyü kır
 if (strstr(line, "}")) break;
     }
   }
}
 // Ork Legi verilerini okuma
 else if (okuma_durumu == 2) {
        if (strstr(line, "kahramanlar")) {
            extract_data(line, "\"kahramanlar\":", temp);
 char *token = strtok(temp, ",");

 while (token && kahraman_index_ork < MAX_KAHRAMAN) {
        sscanf(token, " \"%[^\"]\"", ork_legi->kahramanlar[kahraman_index_ork++]);
 token = strtok(NULL, ",");
 }
 } else if (strstr(line, "canavarlar")) {
     extract_data(line, "\"canavarlar\":", temp);
 char *token = strtok(temp, ",");
 while (token && canavar_index_ork < MAX_CANAVAR) {
        sscanf(token, " \"%[^\"]\"", ork_legi->canavarlar[canavar_index_ork++]);
 token = strtok(NULL, ",");
 }
  }else if (strstr(line, "arastirma_seviyesi")) {
      while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "\"savunma_ustaligi\"")) {
                sscanf(line, " \"savunma_ustaligi\": %d,", &ork_legi->arastirma_seviyesi.savunma_ustaligi);
      } else if (strstr(line, "\"saldiri_gelistirmesi\"")) {
          sscanf(line, " \"saldiri_gelistirmesi\": %d,", &ork_legi->arastirma_seviyesi.saldiri_gelistirmesi);
           } else if (strstr(line, "\"kusatma_ustaligi\"")) {
               sscanf(line, " \"kusatma_ustaligi\": %d,", &ork_legi->arastirma_seviyesi.kusatma_ustaligi);
               } else if (strstr(line, "\"elit_egitim\"")) {
                   sscanf(line, " \"elit_egitim\": %d,", &ork_legi->arastirma_seviyesi.elit_egitim);
               }
 // Araştırma seviyeleri sona erdiğinde döngüyü kır
 if (strstr(line, "}")) break;
     }
  }
 }
 }
 fclose(file);
}
int is_valid_character(char *token) {
    return (strlen(token) > 0 && token[0] != '\0' && token[0] != ' ');
}
void parse_json_unit(const char *filename, Imparatorluk *imparatorluk) {
    FILE *file = fopen(filename, "r");
    if (!file) {
            printf("Dosya açılamadı.\n");
 exit(1);
 }
 char line[256];
 while (fgets(line, sizeof(line), file)) {
// İnsan İmparatorluğu
 if (strstr(line, "\"insan_imparatorlugu\"")) {
        while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
            if (strstr(line, "\"piyadeler\"")) {
                while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
                    if (strstr(line, "\"saldiri\"")) {
                        sscanf(line, " \"saldiri\": %d,", &imparatorluk->unit_insan.piyadeler.saldiri);
                        } else if (strstr(line, "\"savunma\"")) {
                            sscanf(line, " \"savunma\": %d,", &imparatorluk->unit_insan.piyadeler.savunma);
                            } else if (strstr(line, "\"saglik\"")) {
                                sscanf(line, " \"saglik\": %d,", &imparatorluk->unit_insan.piyadeler.saglik);
                                } else if (strstr(line, "\"kritik_sans\"")) {
                                    sscanf(line, " \"kritik_sans\": %d", &imparatorluk->unit_insan.piyadeler.kritik_sans);
                                }
                             }
                       } else if (strstr(line, "\"okcular\"")) {
 while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
        if (strstr(line, "\"saldiri\"")) {
            sscanf(line, " \"saldiri\": %d,", &imparatorluk->unit_insan.okcular.saldiri);
            } else if (strstr(line, "\"savunma\"")) {
                sscanf(line, " \"savunma\": %d,", &imparatorluk->unit_insan.okcular.savunma);
                } else if (strstr(line, "\"saglik\"")) {
                    sscanf(line, " \"saglik\": %d,", &imparatorluk->unit_insan.okcular.saglik);
                    } else if (strstr(line, "\"kritik_sans\"")) {
                        sscanf(line, " \"kritik_sans\": %d", &imparatorluk->unit_insan.okcular.kritik_sans);
                    }
            }
 } else if (strstr(line, "\"suvariler\"")) {
     while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
            if (strstr(line, "\"saldiri\"")) {
                sscanf(line, " \"saldiri\": %d,", &imparatorluk->unit_insan.suvariler.saldiri);} else if (strstr(line, "\"savunma\"")) {
                    sscanf(line, " \"savunma\": %d,", &imparatorluk->unit_insan.suvariler.savunma);
                    } else if (strstr(line, "\"saglik\"")) {
                        sscanf(line, " \"saglik\": %d,", &imparatorluk->unit_insan.suvariler.saglik);
                        } else if (strstr(line, "\"kritik_sans\"")) {
                            sscanf(line, " \"kritik_sans\": %d", &imparatorluk->unit_insan.suvariler.kritik_sans);
                        }
             }
 } else if (strstr(line, "\"kusatma_makineleri\"")) {
     while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
        if (strstr(line, "\"saldiri\"")) {
            sscanf(line, " \"saldiri\": %d,", &imparatorluk->unit_insan.kusatma_makineleri.saldiri);
            } else if (strstr(line, "\"savunma\"")) {
                sscanf(line, " \"savunma\": %d,", &imparatorluk->unit_insan.kusatma_makineleri.savunma);
                } else if (strstr(line, "\"saglik\"")) {
                    sscanf(line, " \"saglik\": %d,", &imparatorluk->unit_insan.kusatma_makineleri.saglik);
                    } else if (strstr(line, "\"kritik_sans\"")) {
                        sscanf(line, " \"kritik_sans\": %d", &imparatorluk->unit_insan.kusatma_makineleri.kritik_sans);
                    }
                }
            }
       }
 }
 // Ork Lejyonu
 if (strstr(line, "\"ork_legi\"")) {
        while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
            if (strstr(line, "\"ork_dovusculeri\"")) {
                    while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
                        if (strstr(line, "\"saldiri\"")) {
                            sscanf(line, " \"saldiri\": %d,", &imparatorluk->unit_org.ork_dovusculeri.saldiri);
            } else if (strstr(line, "\"savunma\"")) {
                sscanf(line, " \"savunma\": %d,", &imparatorluk->unit_org.ork_dovusculeri.savunma);
                } else if (strstr(line, "\"saglik\"")) {
                    sscanf(line, " \"saglik\": %d,", &imparatorluk->unit_org.ork_dovusculeri.saglik);
                 } else if (strstr(line, "\"kritik_sans\"")) {
                     sscanf(line, " \"kritik_sans\": %d", &imparatorluk->unit_org.ork_dovusculeri.kritik_sans);
                  }
                }
 } else if (strstr(line, "\"mizrakcilar\"")) {
 while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
        if (strstr(line, "\"saldiri\"")) {
            sscanf(line, " \"saldiri\": %d,", &imparatorluk->unit_org.mizrakcilar.saldiri);
 } else if (strstr(line, "\"savunma\"")) {
     sscanf(line, " \"savunma\": %d,", &imparatorluk->unit_org.mizrakcilar.savunma);
     } else if (strstr(line, "\"saglik\"")) {
         sscanf(line, " \"saglik\": %d,", &imparatorluk->unit_org.mizrakcilar.saglik);
         } else if (strstr(line, "\"kritik_sans\"")) {
             sscanf(line, " \"kritik_sans\": %d", &imparatorluk->unit_org.mizrakcilar.kritik_sans);
             }
}
} else if (strstr(line, "\"varg_binicileri\"")) {
    while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
            if (strstr(line, "\"saldiri\"")) {
                sscanf(line, " \"saldiri\": %d,", &imparatorluk->unit_org.varg_binicileri.saldiri);
    } else if (strstr(line, "\"savunma\"")) {
        sscanf(line, " \"savunma\": %d,", &imparatorluk->unit_org.varg_binicileri.savunma);
        } else if (strstr(line, "\"saglik\"")) {
            sscanf(line, " \"saglik\": %d,", &imparatorluk->unit_org.varg_binicileri.saglik);
            } else if (strstr(line, "\"kritik_sans\"")) {
                sscanf(line, " \"kritik_sans\": %d", &imparatorluk->unit_org.varg_binicileri.kritik_sans);
                }
   }
 } else if (strstr(line, "\"troller\"")) {
     while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
            if (strstr(line, "\"saldiri\"")) {
                sscanf(line, " \"saldiri\": %d,", &imparatorluk->unit_org.troller.saldiri);
     } else if (strstr(line, "\"savunma\"")) {
         sscanf(line, " \"savunma\": %d,", &imparatorluk->unit_org.troller.savunma);
         } else if (strstr(line, "\"saglik\"")) {
             sscanf(line, " \"saglik\": %d,", &imparatorluk->unit_org.troller.saglik);
             } else if (strstr(line, "\"kritik_sans\"")) {
                 sscanf(line, " \"kritik_sans\": %d", &imparatorluk->unit_org.troller.kritik_sans);
                 }
    }
 }
 }
 }
 }
 fclose(file);
}
void parse_json_research(const char *filename, Imparatorluk *imparatorluk) {
    FILE *file = fopen(filename, "r");
    if (!file) {
            perror("Dosya açılamadı");
    exit(EXIT_FAILURE);
 }
 char line[512];
 char key[20];
 int deger;
 char aciklama[256];
 while (fgets(line, sizeof(line), file)) {
 // Savunma Ustalığı
 if (strstr(line, "\"savunma_ustaligi\"")) {
        for (int i = 1; i <= 3; i++) {
            sprintf(key, "\"seviye_%d\"", i);
            while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
                    if (strstr(line, key)) {
 // "deger" satırını oku
 fgets(line, sizeof(line), file);
 sscanf(line, " \"deger\": \"%d\",", &deger); // Burada %d ile integer olarak okuyoruz
 switch (i) {
     case 1: imparatorluk->arastirma_basarilari.savunma_ustaligi.seviye_1.deger =deger;
     break;
     case 2: imparatorluk->arastirma_basarilari.savunma_ustaligi.seviye_2.deger =deger;
     break;
     case 3: imparatorluk->arastirma_basarilari.savunma_ustaligi.seviye_3.deger =deger;
     break;
 }
 // "aciklama" satırını oku
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", aciklama);
 switch (i) {
     case 1: strcpy(imparatorluk->arastirma_basarilari.savunma_ustaligi.seviye_1.aciklama, aciklama);
     break;
     case 2: strcpy(imparatorluk->arastirma_basarilari.savunma_ustaligi.seviye_2.aciklama, aciklama);
     break;
     case 3: strcpy(imparatorluk->arastirma_basarilari.savunma_ustaligi.seviye_3.aciklama, aciklama);
     break;
 }
                                  }
                                                          }
                                  }
                                                          }
 // Saldırı Geliştirmesi
 else if (strstr(line, "\"saldiri_gelistirmesi\"")) {
        for (int i = 1; i <= 3; i++) {
            sprintf(key, "\"seviye_%d\"", i);
 while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
        if (strstr(line, key)) {
 // "deger" satırını oku
 fgets(line, sizeof(line), file);
 sscanf(line, " \"deger\": \"%d\",", &deger);
 switch (i) {
 case 1: imparatorluk->arastirma_basarilari.saldiri_gelistirmesi.seviye_1.deger =deger;
 break;
 case 2: imparatorluk->arastirma_basarilari.saldiri_gelistirmesi.seviye_2.deger =deger;
 break;
 case 3: imparatorluk->arastirma_basarilari.saldiri_gelistirmesi.seviye_3.deger =deger;
 break;
 }
 // "aciklama" satırını oku
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", aciklama);
 switch (i) {
     case 1: strcpy(imparatorluk->arastirma_basarilari.saldiri_gelistirmesi.seviye_1.aciklama, aciklama);
     break;
     case 2: strcpy(imparatorluk->arastirma_basarilari.saldiri_gelistirmesi.seviye_2.aciklama, aciklama);
     break;
     case 3: strcpy(imparatorluk->arastirma_basarilari.saldiri_gelistirmesi.seviye_3.aciklama, aciklama);
     break;
 }
 }
 }
 }
 }
 // Elit Eğitim
 else if (strstr(line, "\"elit_egitim\"")) {
 for (int i = 1; i <= 3; i++) {
 sprintf(key, "\"seviye_%d\"", i);
 while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
 if (strstr(line, key)) {
 // "deger" satırını oku
 fgets(line, sizeof(line), file);
 sscanf(line, " \"deger\": \"%d\",", &deger);
 switch (i) {
 case 1: imparatorluk->arastirma_basarilari.elit_egitim.seviye_1.deger = deger;
break;
 case 2: imparatorluk->arastirma_basarilari.elit_egitim.seviye_2.deger = deger;
break;
 case 3: imparatorluk->arastirma_basarilari.elit_egitim.seviye_3.deger = deger;
break;
 }
 // "aciklama" satırını oku
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", aciklama);
 switch (i) {
 case 1: strcpy(imparatorluk->arastirma_basarilari.elit_egitim.seviye_1.aciklama,
aciklama); break;
 case 2: strcpy(imparatorluk->arastirma_basarilari.elit_egitim.seviye_2.aciklama,
aciklama); break;
 case 3: strcpy(imparatorluk->arastirma_basarilari.elit_egitim.seviye_3.aciklama,
aciklama); break;
 }
 }
 }
 }
 }
 // Kuşatma Ustalığı
 else if (strstr(line, "\"kusatma_ustaligi\"")) {
 for (int i = 1; i <= 3; i++) {
 sprintf(key, "\"seviye_%d\"", i);
 while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
 if (strstr(line, key)) {
 // "deger" satırını oku
 fgets(line, sizeof(line), file);
 sscanf(line, " \"deger\": \"%d\",", &deger);
 switch (i) {
 case 1: imparatorluk->arastirma_basarilari.kusatma_ustaligi.seviye_1.deger =
deger; break;
 case 2: imparatorluk->arastirma_basarilari.kusatma_ustaligi.seviye_2.deger =
deger; break;
 case 3: imparatorluk->arastirma_basarilari.kusatma_ustaligi.seviye_3.deger =
deger; break;
 }
 // "aciklama" satırını oku
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", aciklama);
 switch (i) {
 case 1: strcpy(imparatorluk->arastirma_basarilari.kusatma_ustaligi.seviye_1.aciklama, aciklama);
 break;
 case 2: strcpy(imparatorluk->arastirma_basarilari.kusatma_ustaligi.seviye_2.aciklama, aciklama);
 break;
 case 3: strcpy(imparatorluk->arastirma_basarilari.kusatma_ustaligi.seviye_3.aciklama, aciklama);
 break;
 }
 }
 }
 }
 }
 }
 fclose(file);
}
void parse_json_yaratik(const char *filename, Imparatorluk *imparatorluk) {
 FILE *file = fopen(filename, "r");
 if (!file) {
 perror("Dosya açılamadı");
 exit(EXIT_FAILURE);
 }
 char line[512];
 while (fgets(line, sizeof(line), file)) {
 // İnsan İmparatorluğu
 if (strstr(line, "\"Ejderha\"")) {
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_degeri\": \"%d\",", &imparatorluk->yaratik_insan.ejderha.etki_degeri);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_turu\": \"%[^\"]\",", imparatorluk->yaratik_insan.ejderha.etki_turu);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", imparatorluk->yaratik_insan.ejderha.aciklama);
 } else if (strstr(line, "\"Agri_Dagi_Devleri\"")) {
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_degeri\": \"%d\",", &imparatorluk->yaratik_insan.agri_dagi_devleri.etki_degeri);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_turu\": \"%[^\"]\",", imparatorluk->yaratik_insan.agri_dagi_devleri.etki_turu);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", imparatorluk->yaratik_insan.agri_dagi_devleri.aciklama);
 } else if (strstr(line, "\"Tepegoz\"")) {
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_degeri\": \"%d\",", &imparatorluk->yaratik_insan.tepegoz.etki_degeri);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_turu\": \"%[^\"]\",", imparatorluk->yaratik_insan.tepegoz.etki_turu);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", imparatorluk->yaratik_insan.tepegoz.aciklama);
 } else if (strstr(line, "\"Karakurt\"")) {
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_degeri\": \"%d\",", &imparatorluk->yaratik_insan.karakurt.etki_degeri);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_turu\": \"%[^\"]\",", imparatorluk->yaratik_insan.karakurt.etki_turu);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", imparatorluk->yaratik_insan.karakurt.aciklama);
 } else if (strstr(line, "\"Samur\"")) {
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_degeri\": \"%d\",", &imparatorluk->yaratik_insan.samur.etki_degeri);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_turu\": \"%[^\"]\",", imparatorluk->yaratik_insan.samur.etki_turu);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", imparatorluk->yaratik_insan.samur.aciklama);
 }
 // Orc Lejyonu
 if (strstr(line, "\"Kara_Troll\"")) {
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_degeri\": \"%d\",", &imparatorluk->yaratik_org.kara_troll.etki_degeri);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_turu\": \"%[^\"]\",", imparatorluk->yaratik_org.kara_troll.etki_turu);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", imparatorluk->yaratik_org.kara_troll.aciklama);
 } else if (strstr(line, "\"Golge_Kurtlari\"")) {
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_degeri\": \"%d\",", &imparatorluk->yaratik_org.golge_kurtlari.etki_degeri);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_turu\": \"%[^\"]\",", imparatorluk->yaratik_org.golge_kurtlari.etki_turu);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", imparatorluk->yaratik_org.golge_kurtlari.aciklama);
 } else if (strstr(line, "\"Camur_Devleri\"")) {
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_degeri\": \"%d\",", &imparatorluk->yaratik_org.camur_devleri.etki_degeri);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_turu\": \"%[^\"]\",", imparatorluk->yaratik_org.camur_devleri.etki_turu);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", imparatorluk->yaratik_org.camur_devleri.aciklama);
 } else if (strstr(line, "\"Ateş_İblisi\"")) {
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_degeri\": \"%d\",", &imparatorluk->yaratik_org.ates_iblisi.etki_degeri);
// Değiştirmediğimiz üye
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_turu\": \"%[^\"]\",", imparatorluk->yaratik_org.ates_iblisi.etki_turu);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", imparatorluk->yaratik_org.ates_iblisi.aciklama);
 } else if (strstr(line, "\"Buz_Devleri\"")) {
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_degeri\": \"%d\",", &imparatorluk->yaratik_org.buz_devleri.etki_degeri);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"etki_turu\": \"%[^\"]\",", imparatorluk->yaratik_org.buz_devleri.etki_turu);
 fgets(line, sizeof(line), file);
 sscanf(line, " \"aciklama\": \"%[^\"]\"", imparatorluk->yaratik_org.buz_devleri.aciklama);
 }
 }
 fclose(file);
}
void parseKahraman(FILE *file, Kahraman *kahraman) {
 char line[512];
 // Kahramanın bilgilerini ayrıştır
 while (fgets(line, sizeof(line), file)) {
 // Kapanış parantezi gelirse döngüden çık
 if (strstr(line, "}")) {
 break;
 }
 // bonus_turu
 if (strstr(line, "\"bonus_turu\":")) {
 sscanf(line, " \"bonus_turu\": \"%[^\"]\"", kahraman->bonus_turu);
 }
 // bonus_degeri
 else if (strstr(line, "\"bonus_degeri\":")) {
 sscanf(line, " \"bonus_degeri\": \"%d\"", &kahraman->bonus_degeri);
 }
 // aciklama
 else if (strstr(line, "\"aciklama\":")) {
 sscanf(line, " \"aciklama\": \"%[^\"]\"", kahraman->aciklama);
 }
 }
}
void parse_json_kahraman(const char *filename, Imparatorluk *imparatorluk) {
    FILE *file = fopen(filename, "r");
    if (!file) {
            fprintf(stderr, "Dosya açılamadı: %s\n", filename);
    exit(1);
 }
 char line[512];
 char kahraman_adi[50];
 imparatorluk->insan_kahraman_sayisi = 0;
 imparatorluk->ork_kahraman_sayisi = 0;
 // Dosyayı satır satır oku
 while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "\"insan_imparatorlugu\"") != NULL) {
            while (fgets(line, sizeof(line), file) && strstr(line, "}") == NULL) {
                if (strstr(line, "{")) {
                    sscanf(line, " \"%[^\"]\"", kahraman_adi);
                    Kahraman temp_kahraman;
                    strcpy(temp_kahraman.ad, kahraman_adi);
                    parseKahraman(file, &temp_kahraman);
                    imparatorluk->insan_kahramanlar[imparatorluk->insan_kahraman_sayisi] =temp_kahraman;
                    imparatorluk->insan_kahraman_sayisi++;
                    }
            }
        }
        else if (strstr(line, "\"ork_legi\"") != NULL) {
                while (fgets(line, sizeof(line), file) && strstr(line, "}") == NULL) {
                    if (strstr(line, "{")) {
                        sscanf(line, " \"%[^\"]\"", kahraman_adi);
                        Kahraman temp_kahraman;
                        strcpy(temp_kahraman.ad, kahraman_adi);
                        parseKahraman(file, &temp_kahraman);
                        imparatorluk->ork_kahramanlar[imparatorluk->ork_kahraman_sayisi] =temp_kahraman;
                        imparatorluk->ork_kahraman_sayisi++;
                        }
                }
        }
}
fclose(file);
}
void print_imparatorluk_data_senaryo(Imparatorluk *imparatorluk, const char *imparatorluk_ismi){
    printf("%s Kahramanları:\n", imparatorluk_ismi);
    for (int i = 0; i < MAX_KAHRAMAN && strlen(imparatorluk->kahramanlar[i]) > 0; i++) {
            printf("%s\n", imparatorluk->kahramanlar[i]);
 }
 printf("\n%s Canavarları:\n", imparatorluk_ismi);
 for (int i = 0; i < MAX_CANAVAR && strlen(imparatorluk->canavarlar[i]) > 0; i++) {
        printf("%s\n", imparatorluk->canavarlar[i]);
 }
 printf("\n%s Birimleri:\n", imparatorluk_ismi);
 if (strcmp(imparatorluk_ismi, "İnsan İmparatorluğu") == 0) {
        printf("Piyadeler: %d\n", imparatorluk->insan_birimleri.piyadeler);
        printf("Okcular: %d\n", imparatorluk->insan_birimleri.okcular);
        printf("Suvariler: %d\n", imparatorluk->insan_birimleri.suvariler);
        printf("Kusatma Makineleri: %d\n", imparatorluk->insan_birimleri.kusatma_makineleri);
        } else {
            printf("Ork Dovuşculeri: %d\n", imparatorluk->ork_birimleri.ork_dovusculeri);
            printf("Mızrakcilar: %d\n", imparatorluk->ork_birimleri.mizrakcilar);
            printf("Varg Binicileri: %d\n", imparatorluk->ork_birimleri.varg_binicileri);
            printf("Troller: %d\n", imparatorluk->ork_birimleri.troller);
            }
            printf("\nArastırma Seviyeleri:\n");
            printf("Savunma Ustalıgı: %d\n", imparatorluk->arastirma_seviyesi.savunma_ustaligi);
            printf("Saldiri Gelistirmesi: %d\n", imparatorluk->arastirma_seviyesi.saldiri_gelistirmesi);
            printf("Kusatma Ustaligi: %d\n", imparatorluk->arastirma_seviyesi.kusatma_ustaligi);
            printf("Elit Egitim: %d\n", imparatorluk->arastirma_seviyesi.elit_egitim);
            }
float toplam_savunma(int birim_sayi,int birim_deger){
    return birim_sayi*birim_deger;
}
float toplam_saldiri(int birim_sayi,int birim_deger){
    return birim_sayi*birim_deger;
}
// Alınan veriyi saklamak için yapı
struct MemoryStruct {
    char *memory;
    size_t size;
};
// Callback fonksiyonu: Veriyi hafızaya yazma
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct*userp) {
    size_t realsize = size * nmemb;
 // Bellekte yer açma
 userp->memory = realloc(userp->memory, userp->size + realsize + 1);
 if (userp->memory == NULL) {
        printf("Yeterli bellek yok!\n");
 return 0;
 }
 // Veriyi kopyalama
 memcpy(&(userp->memory[userp->size]), contents, realsize);
 userp->size += realsize;
 userp->memory[userp->size] = 0; // Null sonlandırma
 return realsize;
}
// Fonksiyon tanımı (JSON verisini dosyaya yazma)
void write_json_to_file(const char *filename, const char *json_data) {
    FILE *file = fopen(filename, "w"); // Dosyayı yazma modunda aç
    if (file) {
        fprintf(file, "%s", json_data);
        fclose(file);
        printf("Veri dosyaya yazildi: %s\n", filename);
        } else {
            printf("Dosya acilmadi: %s\n", filename);
            }
}

int main() {

 setlocale(LC_ALL, "Turkish");
 int secim;
 const char *url;
 Imparatorluk imparatorluk = {0};
 Imparatorluk insan_imparatorlugu = {0}; // Verileri sıfırla
 Imparatorluk ork_legi = {0}; // Verileri sıfırla
 // Kahramanları ve canavarları okumak için çağrı yapıyoruz
 parse_json_unit("C:\\Users\\zeynep\\OneDrive\\Desktop\\Yeniklasor\\unit_types.json", &imparatorluk);
 //print_imparatorluk_data_unit(&imparatorluk);
 parse_json_research("C:\\Users\\zeynep\\OneDrive\\Desktop\\Yeniklasor\\research.json", &imparatorluk);
 // print_imparatorluk_data_research(&imparatorluk);
 parse_json_yaratik("C:\\Users\\zeynep\\OneDrive\\Desktop\\Yeniklasor\\creatures.json", &imparatorluk);
 //print_imparatorluk_data_yaratik(&imparatorluk);
 parse_json_kahraman("C:\\Users\\zeynep\\OneDrive\\Desktop\\Yeniklasor\\heroes.json", &imparatorluk);
 // print_imparatorluk_data_kahraman(&imparatorluk);


 printf("Hangi URL'yi secmek istersiniz (1-10): ");
 scanf("%d", &secim);
 // Seçime göre URL atama
 switch (secim) {
 case 1:
     url = "https://yapbenzet.org.tr/1.json";
     break;
 case 2:
     url = "https://yapbenzet.org.tr/2.json";
     break;
 case 3:
     url = "https://yapbenzet.org.tr/3.json";
     break;
 case 4:
     url = "https://yapbenzet.org.tr/4.json";
     break;
 case 5:
     url = "https://yapbenzet.org.tr/5.json";
     break;
 case 6:
     url = "https://yapbenzet.org.tr/6.json";
     break;
 case 7:
     url = "https://yapbenzet.org.tr/7.json";
     break;
 case 8:
     url = "https://yapbenzet.org.tr/8.json";
     break;
 case 9:
     url = "https://yapbenzet.org.tr/9.json";
     break;
 case 10:
     url = "https://yapbenzet.org.tr/10.json";
     break;
 default:
     printf("Geçersiz seçim, lütfen 1-10 arasında bir sayı girin.\n");
     return 1; // Hata durumu
 }

CURL *curl;
CURLcode res;
// Bellek yapısını başlat
 struct MemoryStruct chunk;
 chunk.memory = malloc(1); // Başlangıçta bellek ayırma
 chunk.size = 0; // Başlangıç boyutu
 // cURL kütüphanesini başlatma
 curl_global_init(CURL_GLOBAL_DEFAULT);
 // cURL oturumunu başlatma
 curl = curl_easy_init();
 if (curl) {
        // URL'yi ayarlama
        curl_easy_setopt(curl, CURLOPT_URL, url);
        // Veriyi yazmak için callback fonksiyonunu ayarlama
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
 // SSL sertifika doğrulamasını kapatma
 curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
 curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
 // İsteği gönderme
 res = curl_easy_perform(curl);
 // Hata kontrolü
 if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() hatasi: %s\n", curl_easy_strerror(res));
 } else {
     printf("Istek basarili!\n");

     // JSON verisini dosyaya yaz
     const char *filename = "output.json";
     write_json_to_file(filename, chunk.memory);// Dosya adı
 // Dosyadan JSON verisini oku
 parse_json_senaryo(filename, &insan_imparatorlugu, &ork_legi);
 dosyadan_verileri_yukle_senaryo(&insan_imparatorlugu, filename);
 dosyadan_verileri_yukle_senaryo(&ork_legi, filename);
 }
 // Temizleme
 curl_easy_cleanup(curl);
 free(chunk.memory); // Belleği serbest bırak
 }
 // cURL kütüphanesini kapat
 curl_global_cleanup();
 //printf("\n===== Ork Legi Bilgileri =====\n");
//print_imparatorluk_data_senaryo(&ork_legi, "Ork Legi");
 //printf("===== İnsan İmparatorlugu Bilgileri =====\n");
//print_imparatorluk_data_senaryo(&insan_imparatorlugu, "İnsan İmparatorluğu");

FILE *savas_raporu = fopen("savas.sim.txt", "a");
    if (savas_raporu == NULL) {
        printf("Dosya acilamadi!\n");
        return 1;
    }

 float toplam_saldiri_insan;
    float toplam_saldiri_ork;
    float toplam_savunma_insan ;
    float toplam_savunma_ork;
   // Toplam saldırıları hesaplama
float toplam_saldiri_okcular = toplam_saldiri(insan_imparatorlugu.insan_birimleri.okcular, imparatorluk.unit_insan.okcular.saldiri);
float toplam_saldiri_piyadeler = toplam_saldiri(insan_imparatorlugu.insan_birimleri.piyadeler, imparatorluk.unit_insan.piyadeler.saldiri);
float toplam_saldiri_suvariler = toplam_saldiri(insan_imparatorlugu.insan_birimleri.suvariler, imparatorluk.unit_insan.suvariler.saldiri);
float toplam_saldiri_kusatma_makineleri = toplam_saldiri(insan_imparatorlugu.insan_birimleri.kusatma_makineleri, imparatorluk.unit_insan.kusatma_makineleri.saldiri);

float toplam_saldiri_ork_dovusculeri = toplam_saldiri(ork_legi.ork_birimleri.ork_dovusculeri, imparatorluk.unit_org.ork_dovusculeri.saldiri);
float toplam_saldiri_mizrakcilar = toplam_saldiri(ork_legi.ork_birimleri.mizrakcilar, imparatorluk.unit_org.mizrakcilar.saldiri);
float toplam_saldiri_varg_biniciler = toplam_saldiri(ork_legi.ork_birimleri.varg_binicileri, imparatorluk.unit_org.varg_binicileri.saldiri);
float toplam_saldiri_troller = toplam_saldiri(ork_legi.ork_birimleri.troller, imparatorluk.unit_org.troller.saldiri);


float toplam_savunma_okcular = toplam_savunma(insan_imparatorlugu.insan_birimleri.okcular, imparatorluk.unit_insan.okcular.savunma);
float toplam_savunma_piyadeler = toplam_savunma(insan_imparatorlugu.insan_birimleri.piyadeler, imparatorluk.unit_insan.piyadeler.savunma);
float toplam_savunma_suvariler = toplam_savunma(insan_imparatorlugu.insan_birimleri.suvariler, imparatorluk.unit_insan.suvariler.savunma);
float toplam_savunma_kusatma_makineleri = toplam_savunma(insan_imparatorlugu.insan_birimleri.kusatma_makineleri, imparatorluk.unit_insan.kusatma_makineleri.savunma);

float toplam_savunma_ork_dovusculeri = toplam_savunma(ork_legi.ork_birimleri.ork_dovusculeri, imparatorluk.unit_org.ork_dovusculeri.savunma);
float toplam_savunma_mizrakcilar = toplam_savunma(ork_legi.ork_birimleri.mizrakcilar, imparatorluk.unit_org.mizrakcilar.savunma);
float toplam_savunma_varg_biniciler = toplam_savunma(ork_legi.ork_birimleri.varg_binicileri, imparatorluk.unit_org.varg_binicileri.savunma);
float toplam_savunma_troller = toplam_savunma(ork_legi.ork_birimleri.troller, imparatorluk.unit_org.troller.savunma);


fprintf(savas_raporu,"İnsan İmparatorlugu  Canavarlari:\n");
    for (int i = 0; i < MAX_CANAVAR && strlen(insan_imparatorlugu.canavarlar[i]) > 0; i++) {
    if (strcmp(insan_imparatorlugu.canavarlar[i], "Ejderha") == 0) {
        imparatorluk.unit_insan.piyadeler.saldiri += imparatorluk.unit_insan.piyadeler.saldiri * (imparatorluk.yaratik_insan.ejderha.etki_degeri / 100.0);
        fprintf(savas_raporu, "Ejderha'nin Piyadelere Saldiri Etkisi : %d\n", imparatorluk.unit_insan.piyadeler.saldiri);
    } else if (strcmp(insan_imparatorlugu.canavarlar[i], "Agri_Dagi_Devleri") == 0) {
        imparatorluk.unit_insan.suvariler.savunma += imparatorluk.unit_insan.suvariler.savunma * (imparatorluk.yaratik_insan.agri_dagi_devleri.etki_degeri / 100.0);
        fprintf(savas_raporu, "Agri Dagi Devlerinin Suvarilere Savunma Etkisi : %d\n", imparatorluk.unit_insan.suvariler.savunma);
    } else if (strcmp(insan_imparatorlugu.canavarlar[i], "Tepegoz") == 0) {
        imparatorluk.unit_insan.okcular.saldiri += imparatorluk.unit_insan.okcular.saldiri * (imparatorluk.yaratik_insan.tepegoz.etki_degeri / 100.0);
        fprintf(savas_raporu, "Tepegoz'un Okculara Saldiri Etkisi : %d\n", imparatorluk.unit_insan.okcular.saldiri);
    } else if (strcmp(insan_imparatorlugu.canavarlar[i], "Karakurt") == 0) {
        imparatorluk.unit_insan.okcular.kritik_sans += imparatorluk.unit_insan.okcular.kritik_sans * (imparatorluk.insan_kahramanlar[3].bonus_degeri / 100.0);
        fprintf(savas_raporu, "Karakurt'un Okculara Kritik Sans Etkisi : %d\n", imparatorluk.unit_insan.okcular.kritik_sans);
    } else if (strcmp(insan_imparatorlugu.canavarlar[i], "Samur") == 0) {
        imparatorluk.unit_insan.piyadeler.savunma += imparatorluk.unit_insan.piyadeler.savunma * (imparatorluk.yaratik_insan.samur.etki_degeri / 100.0);
        fprintf(savas_raporu, "Samur'un Piyadelere Savunma Etkisi : %d\n", imparatorluk.unit_insan.piyadeler.savunma);
    }
}
fprintf(savas_raporu,"Ork Legi Canavarlari:\n");
for (int i = 0; i < MAX_CANAVAR && strlen(ork_legi.canavarlar[i]) > 0; i++) {
    if (strcmp(ork_legi.canavarlar[i], "Kara_Troll") == 0) {
        imparatorluk.unit_org.troller.saldiri += imparatorluk.unit_org.troller.saldiri * (imparatorluk.yaratik_org.kara_troll.etki_degeri / 100.0);
        fprintf(savas_raporu, "Kara Troll'un Troller'e Saldiri Etkisi : %d\n", imparatorluk.unit_org.troller.saldiri);
    } else if (strcmp(ork_legi.canavarlar[i], "Golge_Kurtlari") == 0) {
        imparatorluk.unit_org.varg_binicileri.savunma += imparatorluk.unit_org.varg_binicileri.savunma * (imparatorluk.yaratik_org.golge_kurtlari.etki_degeri / 100.0);
        fprintf(savas_raporu, "Golge Kurtlari'nin Varg Binicilere Savunma Etkisi : %d\n", imparatorluk.unit_org.varg_binicileri.savunma);
    } else if (strcmp(ork_legi.canavarlar[i], "Camur_Devleri") == 0) {
        imparatorluk.unit_org.ork_dovusculeri.savunma += imparatorluk.unit_org.ork_dovusculeri.savunma * (imparatorluk.yaratik_org.camur_devleri.etki_degeri / 100.0);
        fprintf(savas_raporu, "Camur Devleri'nin Ork Dovusculere Savunma Etkisi : %d\n", imparatorluk.unit_org.ork_dovusculeri.savunma);
    } else if (strcmp(ork_legi.canavarlar[i], "Ates_Iblisi") == 0) {
        imparatorluk.unit_org.varg_binicileri.saldiri += imparatorluk.unit_org.varg_binicileri.saldiri * (imparatorluk.yaratik_org.ates_iblisi.etki_degeri / 100.0);
        fprintf(savas_raporu, "Ates Iblisi'nin Varg Binicilere Saldiri Etkisi : %d\n", imparatorluk.unit_org.varg_binicileri.saldiri);
    } else if (strcmp(ork_legi.canavarlar[i], "Makrog_Savas_Beyi") == 0) {
        imparatorluk.unit_org.troller.savunma += imparatorluk.unit_org.troller.savunma * (imparatorluk.yaratik_org.makrog_savas_beyi.etki_degeri / 100.0);
        fprintf(savas_raporu, "Makrog Savas Beyi'nin Troller'e Savunma Etkisi : %d\n", imparatorluk.unit_org.troller.savunma);
    } else if (strcmp(ork_legi.canavarlar[i], "Buz_Devleri") == 0) {
        imparatorluk.unit_org.mizrakcilar.savunma += imparatorluk.unit_org.mizrakcilar.savunma * (imparatorluk.yaratik_org.buz_devleri.etki_degeri / 100.0);
        fprintf(savas_raporu, "Buz Devleri'nin Mizrakcilara Savunma Etkisi : %d\n", imparatorluk.unit_org.mizrakcilar.savunma);
    }
}
fprintf(savas_raporu,"İnsan İmparatorlugu  Kahramanlari:\n");
for (int i = 0; i < MAX_KAHRAMAN && strlen(insan_imparatorlugu.kahramanlar[i]) > 0; i++) {
    if (strcmp(insan_imparatorlugu.kahramanlar[i], imparatorluk.insan_kahramanlar[0].ad) == 0) {
        imparatorluk.unit_insan.piyadeler.savunma += imparatorluk.unit_insan.piyadeler.savunma * (imparatorluk.insan_kahramanlar[0].bonus_degeri / 100.0);
        fprintf(savas_raporu, "Alparslan'in Piyadelere Savunma Bonusu: %d\n", imparatorluk.unit_insan.piyadeler.savunma);
    } else if (strcmp(insan_imparatorlugu.kahramanlar[i], imparatorluk.insan_kahramanlar[1].ad) == 0) {
        imparatorluk.unit_insan.kusatma_makineleri.saldiri += imparatorluk.unit_insan.kusatma_makineleri.saldiri * (imparatorluk.insan_kahramanlar[1].bonus_degeri / 100.0);
        fprintf(savas_raporu, "Fatih Sultan Mehmet'in Kusatma Makinelerine Saldiri Bonusu: %d\n", imparatorluk.unit_insan.kusatma_makineleri.saldiri);
    } else if (strcmp(insan_imparatorlugu.kahramanlar[i], imparatorluk.insan_kahramanlar[2].ad) == 0) {
        imparatorluk.unit_insan.okcular.savunma += imparatorluk.unit_insan.okcular.savunma * (imparatorluk.insan_kahramanlar[2].bonus_degeri / 100.0);
        fprintf(savas_raporu, "Mete Han'in Okculara Savunma Bonusu: %d\n", imparatorluk.unit_insan.okcular.savunma);
    } else if (strcmp(insan_imparatorlugu.kahramanlar[i], imparatorluk.insan_kahramanlar[3].ad) == 0) {
        imparatorluk.unit_insan.suvariler.kritik_sans += imparatorluk.unit_insan.suvariler.kritik_sans * (imparatorluk.insan_kahramanlar[3].bonus_degeri / 100.0);
        fprintf(savas_raporu, "Yavuz Sultan Selim'in Suvarilere Kritik Sans Bonusu: %d\n", imparatorluk.unit_insan.suvariler.kritik_sans);
    } else if (strcmp(insan_imparatorlugu.kahramanlar[i], imparatorluk.insan_kahramanlar[4].ad) == 0) {
        imparatorluk.unit_insan.okcular.saldiri += imparatorluk.unit_insan.okcular.saldiri * (imparatorluk.insan_kahramanlar[4].bonus_degeri / 100.0);
        fprintf(savas_raporu, "Tugrul Bey'in Okculara Saldiri Bonusu: %d\n", imparatorluk.unit_insan.okcular.saldiri);
    }
}
fprintf(savas_raporu,"Ork Legi Kahramanlari:\n");
for (int i = 0; i < MAX_KAHRAMAN && strlen(ork_legi.kahramanlar[i]) > 0; i++) {
    if (strcmp(ork_legi.kahramanlar[i], imparatorluk.ork_kahramanlar[0].ad) == 0) {
        imparatorluk.unit_org.ork_dovusculeri.saldiri += imparatorluk.unit_org.ork_dovusculeri.saldiri * (imparatorluk.ork_kahramanlar[0].bonus_degeri / 100.0);
        fprintf(savas_raporu, "Goruk Vahsi'nin Ork Dovusculerine Saldiri Bonusu: %d\n", imparatorluk.unit_org.ork_dovusculeri.saldiri);
    } else if (strcmp(ork_legi.kahramanlar[i], imparatorluk.ork_kahramanlar[1].ad) == 0) {
        imparatorluk.unit_org.troller.savunma += imparatorluk.unit_org.troller.savunma * (imparatorluk.ork_kahramanlar[1].bonus_degeri / 100.0);
        fprintf(savas_raporu, "Thruk Kemikkiran'in Troller'e Savunma Bonusu: %d\n", imparatorluk.unit_org.troller.savunma);
    } else if (strcmp(ork_legi.kahramanlar[i], imparatorluk.ork_kahramanlar[2].ad) == 0) {
        imparatorluk.unit_org.varg_binicileri.kritik_sans += imparatorluk.unit_org.varg_binicileri.kritik_sans * (imparatorluk.ork_kahramanlar[2].bonus_degeri / 100.0);
        fprintf(savas_raporu, "Vrog Kafakiran'in Varg Binicilerine Kritik Sans Bonusu: %d\n", imparatorluk.unit_org.varg_binicileri.kritik_sans);
    } else if (strcmp(ork_legi.kahramanlar[i], imparatorluk.ork_kahramanlar[3].ad) == 0) {
        imparatorluk.unit_org.ork_dovusculeri.savunma += imparatorluk.unit_org.ork_dovusculeri.savunma * (imparatorluk.ork_kahramanlar[3].bonus_degeri / 100.0);
        imparatorluk.unit_org.troller.savunma += imparatorluk.unit_org.troller.savunma * (imparatorluk.ork_kahramanlar[3].bonus_degeri / 100.0);
        imparatorluk.unit_org.varg_binicileri.savunma += imparatorluk.unit_org.varg_binicileri.savunma * (imparatorluk.ork_kahramanlar[3].bonus_degeri / 100.0);
        imparatorluk.unit_org.mizrakcilar.savunma += imparatorluk.unit_org.mizrakcilar.savunma * (imparatorluk.ork_kahramanlar[3].bonus_degeri / 100.0);
        fprintf(savas_raporu, "Ugar Zalim'in Tum Birimlere Savunma Bonusu: Ork Dovusculeri=%d, Troller=%d, Varg Binicileri=%d, Mizrakcilar=%d\n",
                imparatorluk.unit_org.ork_dovusculeri.savunma,
                imparatorluk.unit_org.troller.savunma,
                imparatorluk.unit_org.varg_binicileri.savunma,
                imparatorluk.unit_org.mizrakcilar.savunma);
    }
}

  if (insan_imparatorlugu.arastirma_seviyesi.savunma_ustaligi == 1) {
     imparatorluk.unit_insan.piyadeler.savunma+=imparatorluk.unit_insan.piyadeler.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_1.deger / 100;
     imparatorluk.unit_insan.okcular.savunma+=imparatorluk.unit_insan.okcular.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_1.deger / 100;
     imparatorluk.unit_insan.suvariler.savunma+=imparatorluk.unit_insan.suvariler.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_1.deger / 100;
     imparatorluk.unit_insan.kusatma_makineleri.savunma+=imparatorluk.unit_insan.kusatma_makineleri.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_1.deger / 100;
} else if (insan_imparatorlugu.arastirma_seviyesi.savunma_ustaligi == 2) {
     imparatorluk.unit_insan.piyadeler.savunma+=imparatorluk.unit_insan.piyadeler.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_2.deger / 100;
     imparatorluk.unit_insan.okcular.savunma+=imparatorluk.unit_insan.okcular.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_2.deger / 100;
     imparatorluk.unit_insan.suvariler.savunma+=imparatorluk.unit_insan.suvariler.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_2.deger / 100;
     imparatorluk.unit_insan.kusatma_makineleri.savunma+=imparatorluk.unit_insan.kusatma_makineleri.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_2.deger / 100;
} else if (insan_imparatorlugu.arastirma_seviyesi.savunma_ustaligi == 3) {
     imparatorluk.unit_insan.piyadeler.savunma+=imparatorluk.unit_insan.piyadeler.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_3.deger / 100;
     imparatorluk.unit_insan.okcular.savunma+=imparatorluk.unit_insan.okcular.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_3.deger / 100;
     imparatorluk.unit_insan.suvariler.savunma+=imparatorluk.unit_insan.suvariler.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_3.deger / 100;
     imparatorluk.unit_insan.kusatma_makineleri.savunma+=imparatorluk.unit_insan.kusatma_makineleri.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_3.deger / 100;
}

if (insan_imparatorlugu.arastirma_seviyesi.saldiri_gelistirmesi == 1) {
    imparatorluk.unit_insan.piyadeler.saldiri+=imparatorluk.unit_insan.piyadeler.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_1.deger / 100;
     imparatorluk.unit_insan.okcular.saldiri+= imparatorluk.unit_insan.okcular.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_1.deger / 100;
     imparatorluk.unit_insan.suvariler.saldiri+=imparatorluk.unit_insan.suvariler.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_1.deger / 100;
    imparatorluk.unit_insan.suvariler.saldiri+= imparatorluk.unit_insan.kusatma_makineleri.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_1.deger / 100;
} else if (insan_imparatorlugu.arastirma_seviyesi.saldiri_gelistirmesi == 2) {
   imparatorluk.unit_insan.piyadeler.saldiri+=imparatorluk.unit_insan.piyadeler.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_2.deger / 100;
     imparatorluk.unit_insan.okcular.saldiri+= imparatorluk.unit_insan.okcular.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_2.deger / 100;
     imparatorluk.unit_insan.suvariler.saldiri+=imparatorluk.unit_insan.suvariler.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_2.deger / 100;
    imparatorluk.unit_insan.suvariler.saldiri+= imparatorluk.unit_insan.kusatma_makineleri.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_2.deger / 100;
} else if (insan_imparatorlugu.arastirma_seviyesi.saldiri_gelistirmesi == 3) {
   imparatorluk.unit_insan.piyadeler.saldiri+=imparatorluk.unit_insan.piyadeler.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_3.deger / 100;
     imparatorluk.unit_insan.okcular.saldiri+= imparatorluk.unit_insan.okcular.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_3.deger / 100;
     imparatorluk.unit_insan.suvariler.saldiri+=imparatorluk.unit_insan.suvariler.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_3.deger / 100;
    imparatorluk.unit_insan.suvariler.saldiri+= imparatorluk.unit_insan.kusatma_makineleri.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_3.deger / 100;
}


if (insan_imparatorlugu.arastirma_seviyesi.kusatma_ustaligi == 1) {
     imparatorluk.unit_insan.kusatma_makineleri.saldiri += imparatorluk.unit_insan.kusatma_makineleri.saldiri* imparatorluk.arastirma_basarilari.kusatma_ustaligi.seviye_1.deger / 100;
} else if (insan_imparatorlugu.arastirma_seviyesi.kusatma_ustaligi == 2) {
 imparatorluk.unit_insan.kusatma_makineleri.saldiri += imparatorluk.unit_insan.kusatma_makineleri.saldiri *   imparatorluk.arastirma_basarilari.kusatma_ustaligi.seviye_2.deger / 100;
} else if (insan_imparatorlugu.arastirma_seviyesi.kusatma_ustaligi == 3) {
  imparatorluk.unit_insan.kusatma_makineleri.saldiri += imparatorluk.unit_insan.kusatma_makineleri.saldiri * imparatorluk.arastirma_basarilari.kusatma_ustaligi.seviye_3.deger / 100;
}

if (insan_imparatorlugu.arastirma_seviyesi.elit_egitim == 1) {
    imparatorluk.unit_insan.piyadeler.kritik_sans += imparatorluk.unit_insan.piyadeler.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_1.deger / 100; // Güncelleme
    imparatorluk.unit_insan.okcular.kritik_sans += imparatorluk.unit_insan.okcular.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_1.deger / 100; // Güncelleme
    imparatorluk.unit_insan.suvariler.kritik_sans += imparatorluk.unit_insan.suvariler.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_1.deger / 100; // Güncelleme
    imparatorluk.unit_insan.suvariler.kritik_sans += imparatorluk.unit_insan.kusatma_makineleri.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_1.deger / 100; // Güncelleme
} else if (insan_imparatorlugu.arastirma_seviyesi.elit_egitim == 2) {
    imparatorluk.unit_insan.piyadeler.kritik_sans += imparatorluk.unit_insan.piyadeler.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_2.deger / 100; // Güncelleme
    imparatorluk.unit_insan.okcular.kritik_sans += imparatorluk.unit_insan.okcular.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_2.deger / 100; // Güncelleme
    imparatorluk.unit_insan.suvariler.kritik_sans += imparatorluk.unit_insan.suvariler.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_2.deger / 100; // Güncelleme
    imparatorluk.unit_insan.suvariler.kritik_sans += imparatorluk.unit_insan.kusatma_makineleri.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_2.deger / 100; // Güncelleme
} else if (insan_imparatorlugu.arastirma_seviyesi.elit_egitim == 3) {
    imparatorluk.unit_insan.piyadeler.kritik_sans += imparatorluk.unit_insan.piyadeler.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_3.deger / 100; // Güncelleme
    imparatorluk.unit_insan.okcular.kritik_sans += imparatorluk.unit_insan.okcular.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_3.deger / 100; // Güncelleme
    imparatorluk.unit_insan.suvariler.kritik_sans += imparatorluk.unit_insan.suvariler.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_3.deger / 100; // Güncelleme
    imparatorluk.unit_insan.suvariler.kritik_sans += imparatorluk.unit_insan.kusatma_makineleri.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_3.deger / 100; // Güncelleme
}


if (ork_legi.arastirma_seviyesi.savunma_ustaligi == 1) {
    imparatorluk.unit_org.ork_dovusculeri.savunma += imparatorluk.unit_org.ork_dovusculeri.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_1.deger / 100; // Güncelleme
    imparatorluk.unit_org.mizrakcilar.savunma += imparatorluk.unit_org.mizrakcilar.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_1.deger / 100; // Güncelleme
    imparatorluk.unit_org.varg_binicileri.savunma += imparatorluk.unit_org.varg_binicileri.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_1.deger / 100; // Güncelleme
    imparatorluk.unit_org.troller.savunma += imparatorluk.unit_org.troller.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_1.deger / 100; // Güncelleme
} else if (ork_legi.arastirma_seviyesi.savunma_ustaligi == 2) {
    imparatorluk.unit_org.ork_dovusculeri.savunma += imparatorluk.unit_org.ork_dovusculeri.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_2.deger / 100; // Güncelleme
    imparatorluk.unit_org.mizrakcilar.savunma += imparatorluk.unit_org.mizrakcilar.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_2.deger / 100; // Güncelleme
    imparatorluk.unit_org.varg_binicileri.savunma += imparatorluk.unit_org.varg_binicileri.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_2.deger / 100; // Güncelleme
    imparatorluk.unit_org.troller.savunma += imparatorluk.unit_org.troller.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_2.deger / 100; // Güncelleme
} else if (ork_legi.arastirma_seviyesi.savunma_ustaligi == 3) {
    imparatorluk.unit_org.ork_dovusculeri.savunma += imparatorluk.unit_org.ork_dovusculeri.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_3.deger / 100; // Güncelleme
    imparatorluk.unit_org.mizrakcilar.savunma += imparatorluk.unit_org.mizrakcilar.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_3.deger / 100; // Güncelleme
    imparatorluk.unit_org.varg_binicileri.savunma += imparatorluk.unit_org.varg_binicileri.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_3.deger / 100; // Güncelleme
    imparatorluk.unit_org.troller.savunma += imparatorluk.unit_org.troller.savunma * imparatorluk.arastirma_basarilari.savunma_ustaligi.seviye_3.deger / 100; // Güncelleme
}

if (ork_legi.arastirma_seviyesi.saldiri_gelistirmesi == 1) {
    imparatorluk.unit_org.ork_dovusculeri.saldiri += imparatorluk.unit_org.ork_dovusculeri.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_1.deger / 100; // Güncelleme
    imparatorluk.unit_org.mizrakcilar.saldiri += imparatorluk.unit_org.mizrakcilar.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_1.deger / 100; // Güncelleme
    imparatorluk.unit_org.varg_binicileri.saldiri += imparatorluk.unit_org.varg_binicileri.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_1.deger / 100; // Güncelleme
    imparatorluk.unit_org.troller.saldiri += imparatorluk.unit_org.troller.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_1.deger / 100; // Güncelleme
} else if (ork_legi.arastirma_seviyesi.saldiri_gelistirmesi == 2) {
    imparatorluk.unit_org.ork_dovusculeri.saldiri += imparatorluk.unit_org.ork_dovusculeri.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_2.deger / 100; // Güncelleme
    imparatorluk.unit_org.mizrakcilar.saldiri += imparatorluk.unit_org.mizrakcilar.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_2.deger / 100; // Güncelleme
    imparatorluk.unit_org.varg_binicileri.saldiri += imparatorluk.unit_org.varg_binicileri.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_2.deger / 100; // Güncelleme
    imparatorluk.unit_org.troller.saldiri += imparatorluk.unit_org.troller.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_2.deger / 100; // Güncelleme
} else if (ork_legi.arastirma_seviyesi.saldiri_gelistirmesi == 3) {
    imparatorluk.unit_org.ork_dovusculeri.saldiri += imparatorluk.unit_org.ork_dovusculeri.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_3.deger / 100; // Güncelleme
    imparatorluk.unit_org.mizrakcilar.saldiri += imparatorluk.unit_org.mizrakcilar.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_3.deger / 100; // Güncelleme
    imparatorluk.unit_org.varg_binicileri.saldiri += imparatorluk.unit_org.varg_binicileri.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_3.deger / 100; // Güncelleme
    imparatorluk.unit_org.troller.saldiri += imparatorluk.unit_org.troller.saldiri * imparatorluk.arastirma_basarilari.saldiri_gelistirmesi.seviye_3.deger / 100; // Güncelleme
}


if (ork_legi.arastirma_seviyesi.elit_egitim == 1) {
    imparatorluk.unit_org.ork_dovusculeri.kritik_sans += imparatorluk.unit_org.ork_dovusculeri.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_1.deger / 100; // Güncelleme
    imparatorluk.unit_org.mizrakcilar.kritik_sans += imparatorluk.unit_org.mizrakcilar.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_1.deger / 100; // Güncelleme
    imparatorluk.unit_org.varg_binicileri.kritik_sans += imparatorluk.unit_org.varg_binicileri.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_1.deger / 100; // Güncelleme
    imparatorluk.unit_org.troller.kritik_sans += imparatorluk.unit_org.troller.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_1.deger / 100; // Güncelleme
} else if (ork_legi.arastirma_seviyesi.elit_egitim == 2) {
    imparatorluk.unit_org.ork_dovusculeri.kritik_sans += imparatorluk.unit_org.ork_dovusculeri.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_2.deger / 100; // Güncelleme
    imparatorluk.unit_org.mizrakcilar.kritik_sans += imparatorluk.unit_org.mizrakcilar.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_2.deger / 100; // Güncelleme
    imparatorluk.unit_org.varg_binicileri.kritik_sans += imparatorluk.unit_org.varg_binicileri.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_2.deger / 100; // Güncelleme
    imparatorluk.unit_org.troller.kritik_sans += imparatorluk.unit_org.troller.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_2.deger / 100; // Güncelleme
} else if (ork_legi.arastirma_seviyesi.elit_egitim == 3) {
    imparatorluk.unit_org.ork_dovusculeri.kritik_sans += imparatorluk.unit_org.ork_dovusculeri.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_3.deger / 100; // Güncelleme
    imparatorluk.unit_org.mizrakcilar.kritik_sans += imparatorluk.unit_org.mizrakcilar.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_3.deger / 100; // Güncelleme
    imparatorluk.unit_org.varg_binicileri.kritik_sans += imparatorluk.unit_org.varg_binicileri.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_3.deger / 100; // Güncelleme
    imparatorluk.unit_org.troller.kritik_sans += imparatorluk.unit_org.troller.kritik_sans * imparatorluk.arastirma_basarilari.elit_egitim.seviye_3.deger / 100; // Güncelleme
}

toplam_saldiri_ork_dovusculeri = toplam_saldiri(ork_legi.ork_birimleri.ork_dovusculeri, imparatorluk.unit_org.ork_dovusculeri.saldiri);
toplam_saldiri_mizrakcilar = toplam_saldiri(ork_legi.ork_birimleri.mizrakcilar, imparatorluk.unit_org.mizrakcilar.saldiri);
toplam_saldiri_varg_biniciler = toplam_saldiri(ork_legi.ork_birimleri.varg_binicileri, imparatorluk.unit_org.varg_binicileri.saldiri);
toplam_saldiri_troller = toplam_saldiri(ork_legi.ork_birimleri.troller, imparatorluk.unit_org.troller.saldiri);


toplam_savunma_ork_dovusculeri = toplam_savunma(ork_legi.ork_birimleri.ork_dovusculeri, imparatorluk.unit_org.ork_dovusculeri.savunma);
toplam_savunma_mizrakcilar = toplam_savunma(ork_legi.ork_birimleri.mizrakcilar, imparatorluk.unit_org.mizrakcilar.savunma);
toplam_savunma_varg_biniciler = toplam_savunma(ork_legi.ork_birimleri.varg_binicileri, imparatorluk.unit_org.varg_binicileri.savunma);
toplam_savunma_troller = toplam_savunma(ork_legi.ork_birimleri.troller, imparatorluk.unit_org.troller.savunma);


  toplam_saldiri_okcular = toplam_saldiri(insan_imparatorlugu.insan_birimleri.okcular, imparatorluk.unit_insan.okcular.saldiri);
             toplam_saldiri_piyadeler = toplam_saldiri(insan_imparatorlugu.insan_birimleri.piyadeler, imparatorluk.unit_insan.piyadeler.saldiri);
             toplam_saldiri_suvariler = toplam_saldiri(insan_imparatorlugu.insan_birimleri.suvariler, imparatorluk.unit_insan.suvariler.saldiri);
             toplam_saldiri_kusatma_makineleri = toplam_saldiri(insan_imparatorlugu.insan_birimleri.kusatma_makineleri, imparatorluk.unit_insan.kusatma_makineleri.saldiri);

             toplam_savunma_okcular = toplam_savunma(insan_imparatorlugu.insan_birimleri.okcular, imparatorluk.unit_insan.okcular.savunma);
            toplam_savunma_piyadeler = toplam_savunma(insan_imparatorlugu.insan_birimleri.piyadeler, imparatorluk.unit_insan.piyadeler.savunma);
            toplam_savunma_suvariler = toplam_savunma(insan_imparatorlugu.insan_birimleri.suvariler, imparatorluk.unit_insan.suvariler.savunma);
            toplam_savunma_kusatma_makineleri = toplam_savunma(insan_imparatorlugu.insan_birimleri.kusatma_makineleri, imparatorluk.unit_insan.kusatma_makineleri.savunma);

 toplam_saldiri_insan=toplam_saldiri_piyadeler+toplam_saldiri_okcular+toplam_saldiri_suvariler+toplam_saldiri_kusatma_makineleri;
 toplam_saldiri_ork=toplam_saldiri_ork_dovusculeri+toplam_saldiri_mizrakcilar+toplam_saldiri_varg_biniciler+toplam_saldiri_troller;
 toplam_savunma_insan = toplam_savunma_piyadeler + toplam_savunma_okcular + toplam_savunma_suvariler + toplam_savunma_kusatma_makineleri;
toplam_savunma_ork = toplam_savunma_ork_dovusculeri + toplam_savunma_mizrakcilar + toplam_savunma_varg_biniciler + toplam_savunma_troller;


const int screenWidth = 1000;
const int screenHeight = 1000;

int insan_sayisi=insan_imparatorlugu.insan_birimleri.okcular+insan_imparatorlugu.insan_birimleri.piyadeler+insan_imparatorlugu.insan_birimleri.suvariler+insan_imparatorlugu.insan_birimleri.kusatma_makineleri;
int ork_sayisi=ork_legi.ork_birimleri.mizrakcilar+ork_legi.ork_birimleri.ork_dovusculeri+ork_legi.ork_birimleri.troller+ork_legi.ork_birimleri.varg_binicileri;

float yuzde_hasar_okcular, hasar_okcular;
int olum_okcular=0;

float yuzde_hasar_piyadeler, hasar_piyadeler;
   int olum_piyadeler=0;

    float yuzde_hasar_suvariler, hasar_suvariler;
    int olum_suvariler=0;

    float yuzde_hasar_kusatma_makineleri, hasar_kusatma_makineleri;
    int olum_kusatma_makineleri=0;

    float yuzde_hasar_ork_dovusculeri, hasar_ork_dovusculeri;
    int olum_ork_dovusculeri=0;

    float yuzde_hasar_mizrakcilar, hasar_mizrakcilar;
    int olum_mizrakcilar=0;

    float yuzde_hasar_varg_binicileri, hasar_varg_binicileri;
    int olum_varg_binicileri=0;

    float yuzde_hasar_troller, hasar_troller;
    int olum_troller=0;
    int adim=1;
    float Net_hasar_insan=0;
    float Net_hasar_ork=0;
    float toplam_saglik_insan=1;
    float toplam_saglik_ork=1;
    int adim_saldiri_insan=1;
    int adim_saldiri_ork=1;
    float saglik_okcular=imparatorluk.unit_insan.okcular.saglik;
    float saglik_piyadeler=imparatorluk.unit_insan.piyadeler.saglik;
    float saglik_kusatma_makineleri=imparatorluk.unit_insan.kusatma_makineleri.saglik;
    float saglik_suvariler=imparatorluk.unit_insan.suvariler.saglik;
    float saglik_ork_dovusculeri=imparatorluk.unit_org.ork_dovusculeri.saglik;
    float saglik_mizrakcilar=imparatorluk.unit_org.mizrakcilar.saglik;
    float saglik_varg_biniciler=imparatorluk.unit_org.varg_binicileri.saglik;
    float saglik_troller=imparatorluk.unit_org.troller.saglik;
    float yuzde_saglik_okcular = (imparatorluk.unit_insan.okcular.saglik / saglik_okcular) * 100.0f;
    float yuzde_saglik_piyadeler = (imparatorluk.unit_insan.piyadeler.saglik / saglik_piyadeler) * 100.0f;
    float yuzde_saglik_kusatma_makineleri = (imparatorluk.unit_insan.kusatma_makineleri.saglik / saglik_kusatma_makineleri) * 100.0f;
    float yuzde_saglik_suvariler = (imparatorluk.unit_insan.suvariler.saglik / saglik_suvariler) * 100.0f;
    float yuzde_saglik_ork_dovusculeri = (imparatorluk.unit_org.ork_dovusculeri.saglik / saglik_ork_dovusculeri) * 100.0f;
    float yuzde_saglik_mizrakcilar = (imparatorluk.unit_org.mizrakcilar.saglik / saglik_mizrakcilar) * 100.0f;
    float yuzde_saglik_varg_biniciler = (imparatorluk.unit_org.varg_binicileri.saglik / saglik_varg_biniciler) * 100.0f;
    float yuzde_saglik_troller = (imparatorluk.unit_org.troller.saglik / saglik_troller) * 100.0f;

    printf("Toplam Insan Saldirisi: %.f\n", toplam_saldiri_insan);
    printf("Toplam Ork Saldirisi: %.f\n", toplam_saldiri_ork);
    printf("Toplam Insan Savunmasi: %.f\n", toplam_savunma_insan);
    printf("Toplam Ork Savunmasi: %.f\n", toplam_savunma_ork);


 SetTraceLogLevel(LOG_NONE);  // Raylib loglarını devre dışı bırak

 InitWindow(screenWidth, screenHeight, "Oyun Baslangici");
    SetTargetFPS(60);  // FPS'i 60 olarak ayarla

    // Görselleri yükleme
    Texture2D textures[] = {
        LoadTexture("okcular.png"),
        LoadTexture("piyadeler.png"),
        LoadTexture("suvariler.png"),
        LoadTexture("kusatma_makineleri.png"),
        LoadTexture("ork_dovusculeri.png"),
        LoadTexture("varg_binicileri.png"),
        LoadTexture("troller.png"),
        LoadTexture("mizrakcilar.png")
    };


    printf("Toplam Insan Saldirisi: %.f\n", toplam_saldiri_insan);
    printf("Toplam Ork Saldirisi: %.f\n", toplam_saldiri_ork);
    printf("Toplam Insan Savunmasi: %.f\n", toplam_savunma_insan);
    printf("Toplam Ork Savunmasi: %.f\n", toplam_savunma_ork);

    // Oyun döngüsü
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);  // Arka planı beyaz yap

        // Izgarayı çiz
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                DrawRectangleLines(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK);
            }
        }

       // İnsan birimlerini yerleştir
    BirimleriYerlestir(textures[0], insan_imparatorlugu.insan_birimleri.okcular, yuzde_saglik_okcular, 0, 0); // Okçular
    BirimleriYerlestir(textures[1], insan_imparatorlugu.insan_birimleri.piyadeler,yuzde_saglik_piyadeler, 2, 0); // Piyadeler
    BirimleriYerlestir(textures[2], insan_imparatorlugu.insan_birimleri.suvariler,yuzde_saglik_suvariler, 4, 0); // Süvariler
    BirimleriYerlestir(textures[3], insan_imparatorlugu.insan_birimleri.kusatma_makineleri, yuzde_saglik_kusatma_makineleri, 6, 0); // Kuşatma makineleri

    // Ork birimlerini yerleştir
    BirimleriYerlestir(textures[4], ork_legi.ork_birimleri.ork_dovusculeri,yuzde_saglik_ork_dovusculeri, 12, 0); // Ork dövüşçüleri
    BirimleriYerlestir(textures[5], ork_legi.ork_birimleri.varg_binicileri,yuzde_saglik_varg_biniciler, 14, 0); // Varg binicileri
    BirimleriYerlestir(textures[6], ork_legi.ork_birimleri.troller, yuzde_saglik_troller, 16, 0); // Troller
    BirimleriYerlestir(textures[7], ork_legi.ork_birimleri.mizrakcilar, yuzde_saglik_mizrakcilar, 18, 0); // Mızrakçılar
        EndDrawing();
    }

    // Kaynakları serbest bırakıyoruz
    for (int i = 0; i < 8; i++) {
        UnloadTexture(textures[i]);
    }


while ((insan_sayisi > 0) && (ork_sayisi > 0) &&(toplam_saglik_insan>0)&&(toplam_saglik_ork>0) ) {
        printf("Adim: %d\n", adim);
        fprintf(savas_raporu, "Adim: %d\n", adim);

        if (adim % 2 == 1) { // İnsanların saldırdığı adımlar
            printf("Insanlarin saldirisi:  %d. saldiri\n",adim_saldiri_insan);
            fprintf(savas_raporu,"Insanlarin saldirisi:  %d. saldiri\n",adim_saldiri_insan);
            printf("Insan sayisi: %d\n", insan_sayisi);
            printf("Ork sayisi: %d\n", ork_sayisi);

if (adim_saldiri_insan %(imparatorluk.unit_insan.okcular.kritik_sans)==0) {
    toplam_saldiri_okcular += (toplam_saldiri_okcular * 50 / 100);
}

if (adim_saldiri_insan % (imparatorluk.unit_insan.piyadeler.kritik_sans)==0) {
    toplam_saldiri_piyadeler += (toplam_saldiri_piyadeler * 50/ 100);
}

if (adim_saldiri_insan % ( imparatorluk.unit_insan.suvariler.kritik_sans)==0) {
    toplam_saldiri_suvariler += (toplam_saldiri_suvariler * 50 / 100);
}



    if (adim % 5 == 0) {
    // Okçular için
    toplam_saldiri_okcular = toplam_saldiri_okcular - toplam_saldiri_okcular * 0.1;
    toplam_savunma_okcular = toplam_savunma_okcular - toplam_savunma_okcular * 0.1;

    // Piyadeler için
    toplam_saldiri_piyadeler = toplam_saldiri_piyadeler - toplam_saldiri_piyadeler * 0.1;
    toplam_savunma_piyadeler = toplam_savunma_piyadeler - toplam_savunma_piyadeler * 0.1;

    // Süvariler için
    toplam_saldiri_suvariler = toplam_saldiri_suvariler - toplam_saldiri_suvariler * 0.1;
    toplam_savunma_suvariler = toplam_savunma_suvariler - toplam_savunma_suvariler * 0.1;

    // Kuşatma Makineleri için
    toplam_saldiri_kusatma_makineleri = toplam_saldiri_kusatma_makineleri - toplam_saldiri_kusatma_makineleri * 0.1;
    toplam_savunma_kusatma_makineleri = toplam_savunma_kusatma_makineleri - toplam_savunma_kusatma_makineleri * 0.1;

    // Ork Dövüşçüleri için
    toplam_saldiri_ork_dovusculeri = toplam_saldiri_ork_dovusculeri - toplam_saldiri_ork_dovusculeri * 0.1;
    toplam_savunma_ork_dovusculeri = toplam_savunma_ork_dovusculeri - toplam_savunma_ork_dovusculeri * 0.1;

    // Mızrakçılar için
    toplam_saldiri_mizrakcilar = toplam_saldiri_mizrakcilar - toplam_saldiri_mizrakcilar * 0.1;
    toplam_savunma_mizrakcilar = toplam_savunma_mizrakcilar - toplam_savunma_mizrakcilar * 0.1;

    // Varg Binicileri için
    toplam_saldiri_varg_biniciler = toplam_saldiri_varg_biniciler - toplam_saldiri_varg_biniciler * 0.1;
    toplam_savunma_varg_biniciler = toplam_savunma_varg_biniciler - toplam_savunma_varg_biniciler * 0.1;

    // Troller için
    toplam_saldiri_troller = toplam_saldiri_troller - toplam_saldiri_troller * 0.1;
    toplam_savunma_troller = toplam_savunma_troller - toplam_savunma_troller * 0.1;
}
toplam_saldiri_insan=toplam_saldiri_piyadeler+toplam_saldiri_okcular+toplam_saldiri_suvariler+toplam_saldiri_kusatma_makineleri;
toplam_saldiri_ork=toplam_saldiri_ork_dovusculeri+toplam_saldiri_mizrakcilar+toplam_saldiri_varg_biniciler+toplam_saldiri_troller;
toplam_savunma_insan = toplam_savunma_piyadeler + toplam_savunma_okcular + toplam_savunma_suvariler + toplam_savunma_kusatma_makineleri;
toplam_savunma_ork = toplam_savunma_ork_dovusculeri + toplam_savunma_mizrakcilar + toplam_savunma_varg_biniciler + toplam_savunma_troller;

    printf("Toplam Insan Saldirisi: %.f\n", toplam_saldiri_insan);
    printf("Toplam Ork Saldirisi: %.f\n", toplam_saldiri_ork);
    printf("Toplam Insan Savunmasi: %.f\n", toplam_savunma_insan);
    printf("Toplam Ork Savunmasi: %.f\n", toplam_savunma_ork);

           // Net hasarın pozitif kalmasını sağlamak için minimum hasar uygulaması
            Net_hasar_ork = toplam_saldiri_insan * (1 - (toplam_savunma_ork / (float)(toplam_saldiri_insan)));


            // Net hasar negatif veya sıfır ise, minimum hasar uygulanabilir
            if (Net_hasar_ork < 0) {
                Net_hasar_ork= Net_hasar_ork*(-1);
            }

            printf("Net Hasar (Orklar): %.f\n", Net_hasar_ork);
            fprintf(savas_raporu,"Net Hasar (Orklar): %.f\n", Net_hasar_ork);


 if (ork_legi.ork_birimleri.ork_dovusculeri > 0 && imparatorluk.unit_org.ork_dovusculeri.saglik > 0) {


    yuzde_hasar_ork_dovusculeri = toplam_savunma_ork_dovusculeri / toplam_savunma_ork;
    hasar_ork_dovusculeri = yuzde_hasar_ork_dovusculeri * Net_hasar_ork;
    olum_ork_dovusculeri = (int)(hasar_ork_dovusculeri / imparatorluk.unit_org.ork_dovusculeri.saglik);
        imparatorluk.unit_org.ork_dovusculeri.saglik -= hasar_ork_dovusculeri / ork_legi.ork_birimleri.ork_dovusculeri;
   ork_legi.ork_birimleri.ork_dovusculeri -= olum_ork_dovusculeri;

    if(ork_legi.ork_birimleri.ork_dovusculeri<0)
        ork_legi.ork_birimleri.ork_dovusculeri=0;

}



// Ork Mizrakçıları
if (ork_legi.ork_birimleri.mizrakcilar > 0 && imparatorluk.unit_org.mizrakcilar.saglik > 0) {

    yuzde_hasar_mizrakcilar = toplam_savunma_mizrakcilar / toplam_savunma_ork;
    hasar_mizrakcilar = yuzde_hasar_mizrakcilar * Net_hasar_ork;
    olum_mizrakcilar = (int)(hasar_mizrakcilar / imparatorluk.unit_org.mizrakcilar.saglik);
     imparatorluk.unit_org.mizrakcilar.saglik -= hasar_mizrakcilar / ork_legi.ork_birimleri.mizrakcilar;
    ork_legi.ork_birimleri.mizrakcilar -= olum_mizrakcilar;

      if(ork_legi.ork_birimleri.mizrakcilar<0)
        ork_legi.ork_birimleri.mizrakcilar=0;
}

// Ork Varg Binicileri
if (ork_legi.ork_birimleri.varg_binicileri > 0 && imparatorluk.unit_org.varg_binicileri.saglik > 0) {

    yuzde_hasar_varg_binicileri = toplam_savunma_varg_biniciler / toplam_savunma_ork;
    hasar_varg_binicileri = yuzde_hasar_varg_binicileri * Net_hasar_ork;
    olum_varg_binicileri = (int)(hasar_varg_binicileri / imparatorluk.unit_org.varg_binicileri.saglik);
    imparatorluk.unit_org.varg_binicileri.saglik -= hasar_varg_binicileri / ork_legi.ork_birimleri.varg_binicileri;
    ork_legi.ork_birimleri.varg_binicileri -= olum_varg_binicileri;
       if(ork_legi.ork_birimleri.varg_binicileri<0)
        ork_legi.ork_birimleri.varg_binicileri=0;
}

// Ork Troller
if (ork_legi.ork_birimleri.troller > 0 && imparatorluk.unit_org.troller.saglik > 0) {

    yuzde_hasar_troller = toplam_savunma_troller / toplam_savunma_ork;
    hasar_troller = yuzde_hasar_troller * Net_hasar_ork;
    olum_troller = (int)(hasar_troller / imparatorluk.unit_org.troller.saglik);
    imparatorluk.unit_org.troller.saglik -= hasar_troller / ork_legi.ork_birimleri.troller;
    ork_legi.ork_birimleri.troller -= olum_troller;
       if(ork_legi.ork_birimleri.troller<0)
        ork_legi.ork_birimleri.troller=0;
}

if (imparatorluk.unit_org.ork_dovusculeri.saglik <= 0) {
    printf("Ork dovusculeri saglik 0 alti\n");
    ork_legi.ork_birimleri.ork_dovusculeri = 0;
    imparatorluk.unit_org.ork_dovusculeri.saglik =0;
}

if (imparatorluk.unit_org.mizrakcilar.saglik <= 0) {
    printf("Mizrakcilar saglik 0 alti\n");
    ork_legi.ork_birimleri.mizrakcilar = 0;
    imparatorluk.unit_org.mizrakcilar.saglik =0;
}
if (imparatorluk.unit_org.varg_binicileri.saglik <= 0) {
    printf("Varg binicileri saglik 0 alti\n");
    ork_legi.ork_birimleri.varg_binicileri = 0;
    imparatorluk.unit_org.varg_binicileri.saglik =0;
}
if (imparatorluk.unit_org.troller.saglik <= 0) {
    printf("Troller saglik 0 alti\n");
    ork_legi.ork_birimleri.troller = 0;
    imparatorluk.unit_org.troller.saglik =0;
}
printf("Ork dovusculeri kaybi: %d\n", olum_ork_dovusculeri);
fprintf(savas_raporu,"Ork dovusculeri kaybi: %d\n", olum_ork_dovusculeri);
printf("Mizrakcilar kaybi: %d\n", olum_mizrakcilar);
fprintf(savas_raporu,"Mizrakcilar kaybi: %d\n", olum_mizrakcilar);
printf("Varg binicileri kaybi: %d\n", olum_varg_binicileri);
fprintf(savas_raporu,"Varg binicileri kaybi: %d\n", olum_varg_binicileri);
printf("Troller kaybi: %d\n", olum_troller);
fprintf(savas_raporu,"Troller kaybi: %d\n", olum_troller);


// Ork dövüşçüleri sağlık durumu
printf("Ork Dovusculeri Saglik: %d\n", imparatorluk.unit_org.ork_dovusculeri.saglik);
fprintf(savas_raporu, "Ork Dovusculeri Saglik: %d\n", imparatorluk.unit_org.ork_dovusculeri.saglik);

// Mızrakçıların sağlık durumu
printf("Mizrakcilar Saglik: %d\n", imparatorluk.unit_org.mizrakcilar.saglik);
fprintf(savas_raporu, "Mizrakcilar Saglik: %d\n", imparatorluk.unit_org.mizrakcilar.saglik);

// Varg binicilerinin sağlık durumu
printf("Varg Binicileri Saglik: %d\n", imparatorluk.unit_org.varg_binicileri.saglik);
fprintf(savas_raporu, "Varg Binicileri Saglik: %d\n", imparatorluk.unit_org.varg_binicileri.saglik);

// Trollerin sağlık durumu
printf("Troller Saglik: %d\n", imparatorluk.unit_org.troller.saglik);
fprintf(savas_raporu, "Troller Saglik: %d\n", imparatorluk.unit_org.troller.saglik);
// Ork dövüşçüleri için saldırı toplamını güncelle
if (toplam_saldiri_ork_dovusculeri > toplam_saldiri(olum_ork_dovusculeri, imparatorluk.unit_org.ork_dovusculeri.saldiri)) {
    toplam_saldiri_ork_dovusculeri -= toplam_saldiri(olum_ork_dovusculeri, imparatorluk.unit_org.ork_dovusculeri.saldiri);
} else {
    toplam_saldiri_ork_dovusculeri = 0;
}

// Mızrakçılar için saldırı toplamını güncelle
if (toplam_saldiri_mizrakcilar > toplam_saldiri(olum_mizrakcilar, imparatorluk.unit_org.mizrakcilar.saldiri)) {
    toplam_saldiri_mizrakcilar -= toplam_saldiri(olum_mizrakcilar, imparatorluk.unit_org.mizrakcilar.saldiri);
} else {
    toplam_saldiri_mizrakcilar = 0;
}

// Varg binicileri için saldırı toplamını güncelle
if (toplam_saldiri_varg_biniciler > toplam_saldiri(olum_varg_binicileri, imparatorluk.unit_org.varg_binicileri.saldiri)) {
    toplam_saldiri_varg_biniciler -= toplam_saldiri(olum_varg_binicileri, imparatorluk.unit_org.varg_binicileri.saldiri);
} else {
    toplam_saldiri_varg_biniciler = 0;
}

// Troller için saldırı toplamını güncelle
if (toplam_saldiri_troller > toplam_saldiri(olum_troller, imparatorluk.unit_org.troller.saldiri)) {
    toplam_saldiri_troller -= toplam_saldiri(olum_troller, imparatorluk.unit_org.troller.saldiri);
} else {
    toplam_saldiri_troller = 0;
}

if (toplam_savunma_ork_dovusculeri > toplam_savunma(olum_ork_dovusculeri, imparatorluk.unit_org.ork_dovusculeri.savunma)) {
    toplam_savunma_ork_dovusculeri -= toplam_savunma(olum_ork_dovusculeri, imparatorluk.unit_org.ork_dovusculeri.savunma);
} else {
    toplam_savunma_ork_dovusculeri = 0;
}

// Mızrakçılar için savunma toplamını güncelle
if (toplam_savunma_mizrakcilar > toplam_savunma(olum_mizrakcilar, imparatorluk.unit_org.mizrakcilar.savunma)) {
    toplam_savunma_mizrakcilar -= toplam_savunma(olum_mizrakcilar, imparatorluk.unit_org.mizrakcilar.savunma);
} else {
    toplam_savunma_mizrakcilar = 0;
}

// Varg binicileri için savunma toplamını güncelle
if (toplam_savunma_varg_biniciler > toplam_savunma(olum_varg_binicileri, imparatorluk.unit_org.varg_binicileri.savunma)) {
    toplam_savunma_varg_biniciler -= toplam_savunma(olum_varg_binicileri, imparatorluk.unit_org.varg_binicileri.savunma);
} else {
    toplam_savunma_varg_biniciler = 0;
}

// Troller için savunma toplamını güncelle
if (toplam_savunma_troller > toplam_savunma(olum_troller, imparatorluk.unit_org.troller.savunma)) {
    toplam_savunma_troller -= toplam_savunma(olum_troller, imparatorluk.unit_org.troller.savunma);
} else {
    toplam_savunma_troller = 0;
}
toplam_saldiri_ork = toplam_saldiri_mizrakcilar + toplam_saldiri_ork_dovusculeri + toplam_saldiri_varg_biniciler + toplam_saldiri_troller;
toplam_savunma_ork = toplam_savunma_ork_dovusculeri + toplam_savunma_mizrakcilar + toplam_savunma_varg_biniciler + toplam_savunma_troller;



    printf("Toplam Saldiri Degerleri:\n");
    printf("Ork Dovusculeri: %.f\n", toplam_saldiri_ork_dovusculeri);
    printf("Mızrakcilar: %.f\n", toplam_saldiri_mizrakcilar);
    printf("Varg Binicileri: %.f\n", toplam_saldiri_varg_biniciler);
    printf("Troller: %.f\n", toplam_saldiri_troller);
    printf("Toplam Saldiri: %.f\n", toplam_saldiri_ork);

    printf("\nToplam Savunma Degerleri:\n");
    printf("Ork Dovusculeri: %.f\n", toplam_savunma_ork_dovusculeri);
    printf("Mızrakcilar: %.f\n", toplam_savunma_mizrakcilar);
    printf("Varg Binicileri: %.f\n", toplam_savunma_varg_biniciler);
    printf("Troller: %.f\n", toplam_savunma_troller);
    printf("Toplam Savunma: %.f\n", toplam_savunma_ork);
    toplam_saglik_ork=imparatorluk.unit_org.troller.saglik +imparatorluk.unit_org.varg_binicileri.saglik+imparatorluk.unit_org.mizrakcilar.saglik+imparatorluk.unit_org.ork_dovusculeri.saglik;

            // Ork sayısını güncelle
            ork_sayisi = ork_legi.ork_birimleri.mizrakcilar + ork_legi.ork_birimleri.ork_dovusculeri +
                         ork_legi.ork_birimleri.troller + ork_legi.ork_birimleri.varg_binicileri;
                adim_saldiri_insan++;
            printf("--------------------------\n");

        } else {
            // Orkların saldırdığı adımlar
            printf("Orklarin saldirisi: %d. saldiri\n",adim_saldiri_ork);
            fprintf(savas_raporu,"Orklarin saldirisi: %d. saldiri\n",adim_saldiri_ork);
            printf("Insan sayisi: %d\n", insan_sayisi);
            printf("Ork sayisi: %d\n", ork_sayisi);

if (adim_saldiri_ork % ( imparatorluk.unit_org.ork_dovusculeri.kritik_sans)==0) {
    toplam_saldiri_ork_dovusculeri += (toplam_saldiri_ork_dovusculeri * 50 / 100);
}

if (adim_saldiri_ork %( imparatorluk.unit_org.mizrakcilar.kritik_sans)==0) {
    toplam_saldiri_mizrakcilar += (toplam_saldiri_mizrakcilar * 50 / 100);
}

if (adim_saldiri_ork % (imparatorluk.unit_org.varg_binicileri.kritik_sans)==0) {
    toplam_saldiri_varg_biniciler += (toplam_saldiri_varg_biniciler * 50 / 100);
}

if (adim_saldiri_ork % ( imparatorluk.unit_org.troller.kritik_sans)==0) {
    toplam_saldiri_troller += (toplam_saldiri_troller * 50 / 100);
}


if (adim % 5 == 0) {
    // Okçular için
    toplam_saldiri_okcular = toplam_saldiri_okcular - toplam_saldiri_okcular * 0.1;
    toplam_savunma_okcular = toplam_savunma_okcular - toplam_savunma_okcular * 0.1;

    // Piyadeler için
    toplam_saldiri_piyadeler = toplam_saldiri_piyadeler - toplam_saldiri_piyadeler * 0.1;
    toplam_savunma_piyadeler = toplam_savunma_piyadeler - toplam_savunma_piyadeler * 0.1;

    // Süvariler için
    toplam_saldiri_suvariler = toplam_saldiri_suvariler - toplam_saldiri_suvariler * 0.1;
    toplam_savunma_suvariler = toplam_savunma_suvariler - toplam_savunma_suvariler * 0.1;

    // Kuşatma Makineleri için
    toplam_saldiri_kusatma_makineleri = toplam_saldiri_kusatma_makineleri - toplam_saldiri_kusatma_makineleri * 0.1;
    toplam_savunma_kusatma_makineleri = toplam_savunma_kusatma_makineleri - toplam_savunma_kusatma_makineleri * 0.1;

    // Ork Dövüşçüleri için
    toplam_saldiri_ork_dovusculeri = toplam_saldiri_ork_dovusculeri - toplam_saldiri_ork_dovusculeri * 0.1;
    toplam_savunma_ork_dovusculeri = toplam_savunma_ork_dovusculeri - toplam_savunma_ork_dovusculeri * 0.1;

    // Mızrakçılar için
    toplam_saldiri_mizrakcilar = toplam_saldiri_mizrakcilar - toplam_saldiri_mizrakcilar * 0.1;
    toplam_savunma_mizrakcilar = toplam_savunma_mizrakcilar - toplam_savunma_mizrakcilar * 0.1;

    // Varg Binicileri için
    toplam_saldiri_varg_biniciler = toplam_saldiri_varg_biniciler - toplam_saldiri_varg_biniciler * 0.1;
    toplam_savunma_varg_biniciler = toplam_savunma_varg_biniciler - toplam_savunma_varg_biniciler * 0.1;

    // Troller için
    toplam_saldiri_troller = toplam_saldiri_troller - toplam_saldiri_troller * 0.1;
    toplam_savunma_troller = toplam_savunma_troller - toplam_savunma_troller * 0.1;
}
toplam_saldiri_insan=toplam_saldiri_piyadeler+toplam_saldiri_okcular+toplam_saldiri_suvariler+toplam_saldiri_kusatma_makineleri;
 toplam_saldiri_ork=toplam_saldiri_ork_dovusculeri+toplam_saldiri_mizrakcilar+toplam_saldiri_varg_biniciler+toplam_saldiri_troller;
 toplam_savunma_insan = toplam_savunma_piyadeler + toplam_savunma_okcular + toplam_savunma_suvariler + toplam_savunma_kusatma_makineleri;
toplam_savunma_ork = toplam_savunma_ork_dovusculeri + toplam_savunma_mizrakcilar + toplam_savunma_varg_biniciler + toplam_savunma_troller;

    printf("Toplam Insan Saldirisi: %.f\n", toplam_saldiri_insan);
    printf("Toplam Ork Saldirisi: %.f\n", toplam_saldiri_ork);
    printf("Toplam Insan Savunmasi: %.f\n", toplam_savunma_insan);
    printf("Toplam Ork Savunmasi: %.f\n", toplam_savunma_ork);

            Net_hasar_insan = toplam_saldiri_ork * (1 -( toplam_savunma_insan / (float)(toplam_saldiri_ork )));


            // Net hasar negatif veya sıfır ise, minimum hasar uygulanabilir
            if (Net_hasar_insan <= 0) {
               Net_hasar_insan=Net_hasar_insan*(-1);
            }
            printf("Net Hasar (Insanlar): %.f\n", Net_hasar_insan);
            fprintf(savas_raporu,"Net Hasar (Insanlar): %.f\n", Net_hasar_insan);

            // Okçular için hasar ve ölümleri hesapla
       if (insan_imparatorlugu.insan_birimleri.okcular > 0 && imparatorluk.unit_insan.okcular.saglik > 0) {

    yuzde_hasar_okcular = toplam_savunma_okcular / toplam_savunma_insan;
    hasar_okcular = yuzde_hasar_okcular * Net_hasar_insan;
    olum_okcular = (int)(hasar_okcular / imparatorluk.unit_insan.okcular.saglik);
    imparatorluk.unit_insan.okcular.saglik -= hasar_okcular / insan_imparatorlugu.insan_birimleri.okcular;
    insan_imparatorlugu.insan_birimleri.okcular -= olum_okcular;

    if (insan_imparatorlugu.insan_birimleri.okcular < 0)
        insan_imparatorlugu.insan_birimleri.okcular = 0;
}


// Piyadeler için hasar ve ölümleri hesapla
if (insan_imparatorlugu.insan_birimleri.piyadeler > 0 && imparatorluk.unit_insan.piyadeler.saglik > 0) {

    yuzde_hasar_piyadeler = toplam_savunma_piyadeler / toplam_savunma_insan;
    hasar_piyadeler = yuzde_hasar_piyadeler * Net_hasar_insan;
    olum_piyadeler = (int)(hasar_piyadeler / imparatorluk.unit_insan.piyadeler.saglik);
      imparatorluk.unit_insan.piyadeler.saglik -= hasar_piyadeler / insan_imparatorlugu.insan_birimleri.piyadeler;
    insan_imparatorlugu.insan_birimleri.piyadeler -= olum_piyadeler;

    if (insan_imparatorlugu.insan_birimleri.piyadeler < 0)
        insan_imparatorlugu.insan_birimleri.piyadeler = 0;
}




// Süvariler için hasar ve ölümleri hesapla
if (insan_imparatorlugu.insan_birimleri.suvariler > 0 && imparatorluk.unit_insan.suvariler.saglik > 0) {

    yuzde_hasar_suvariler = toplam_savunma_suvariler / toplam_savunma_insan;
    hasar_suvariler = yuzde_hasar_suvariler * Net_hasar_insan;
    olum_suvariler = (int)(hasar_suvariler / imparatorluk.unit_insan.suvariler.saglik);
    imparatorluk.unit_insan.suvariler.saglik -= hasar_suvariler / insan_imparatorlugu.insan_birimleri.suvariler;
    insan_imparatorlugu.insan_birimleri.suvariler -= olum_suvariler;

    if (insan_imparatorlugu.insan_birimleri.suvariler < 0)
        insan_imparatorlugu.insan_birimleri.suvariler =0;
}


// Kuşatma makineleri için hasar ve ölümleri hesapla
if (insan_imparatorlugu.insan_birimleri.kusatma_makineleri > 0 && imparatorluk.unit_insan.kusatma_makineleri.saglik > 0) {

    yuzde_hasar_kusatma_makineleri = toplam_savunma_kusatma_makineleri / toplam_savunma_insan;
    hasar_kusatma_makineleri = yuzde_hasar_kusatma_makineleri * Net_hasar_insan;
    olum_kusatma_makineleri = (int)(hasar_kusatma_makineleri / imparatorluk.unit_insan.kusatma_makineleri.saglik);
    imparatorluk.unit_insan.kusatma_makineleri.saglik -= hasar_kusatma_makineleri / insan_imparatorlugu.insan_birimleri.kusatma_makineleri;
    insan_imparatorlugu.insan_birimleri.kusatma_makineleri -= olum_kusatma_makineleri;


    if (insan_imparatorlugu.insan_birimleri.kusatma_makineleri < 0)
        insan_imparatorlugu.insan_birimleri.kusatma_makineleri = 0;
}
// Kuşatma Makineleri ve diğer insan birimleri için sağlık durumu kontrolü
if (imparatorluk.unit_insan.kusatma_makineleri.saglik <= 0) {
    printf("Kusatma makineleri saglik 0 alti\n");
    insan_imparatorlugu.insan_birimleri.kusatma_makineleri = 0;
    imparatorluk.unit_insan.kusatma_makineleri.saglik = 0;
}

if (imparatorluk.unit_insan.suvariler.saglik <= 0) {
    printf("Suvariler saglik 0 alti\n");
    insan_imparatorlugu.insan_birimleri.suvariler = 0;
    imparatorluk.unit_insan.suvariler.saglik =0;
}

if (imparatorluk.unit_insan.piyadeler.saglik <= 0) {
    printf("Piyadeler saglik 0 alti\n");
    insan_imparatorlugu.insan_birimleri.piyadeler = 0;
    imparatorluk.unit_insan.piyadeler.saglik =0;
}

if (imparatorluk.unit_insan.okcular.saglik <= 0) {
    printf("Okcular saglik 0 alti\n");
    insan_imparatorlugu.insan_birimleri.okcular = 0;
    imparatorluk.unit_insan.okcular.saglik =0;

}
            // Kalan insan birimlerini yazdır
            printf("Piyadeler kaybi: %d\n", olum_piyadeler);
            fprintf(savas_raporu,"Piyadeler kaybi: %d\n", olum_piyadeler);
            printf("Okcular kaybi: %d\n", olum_okcular);
            fprintf(savas_raporu,"Okcular kaybi: %d\n", olum_okcular);
            printf("Suvariler kaybi: %d\n", olum_suvariler);
            fprintf(savas_raporu,"Suvariler kaybi: %d\n", olum_suvariler);
            printf("Kusatma makineleri kaybi: %d\n", olum_kusatma_makineleri);
            fprintf(savas_raporu,"Kusatma makineleri kaybi: %d\n", olum_kusatma_makineleri);
            // Okçuların sağlık durumu
printf("Insan Okcular Saglik: %d\n", imparatorluk.unit_insan.okcular.saglik);
fprintf(savas_raporu, "Insan Okcular Saglik: %d\n", imparatorluk.unit_insan.okcular.saglik);

// Piyadelerin sağlık durumu
printf("Insan Piyadeler Saglik: %d\n", imparatorluk.unit_insan.piyadeler.saglik);
fprintf(savas_raporu, "Insan Piyadeler Saglik: %d\n", imparatorluk.unit_insan.piyadeler.saglik);

// Süvarilerin sağlık durumu
printf("Insan Suvariler Saglik: %d\n", imparatorluk.unit_insan.suvariler.saglik);
fprintf(savas_raporu, "Insan Suvariler Saglik: %d\n", imparatorluk.unit_insan.suvariler.saglik);

// Kuşatma makinelerinin sağlık durumu
printf("Insan Kusatma Makineleri Saglik: %d\n", imparatorluk.unit_insan.kusatma_makineleri.saglik);
fprintf(savas_raporu, "Insan Kusatma Makineleri Saglik: %d\n", imparatorluk.unit_insan.kusatma_makineleri.saglik);

// Piyadeler için saldırı toplamını güncelle
if (toplam_saldiri_piyadeler > toplam_saldiri(olum_piyadeler, imparatorluk.unit_insan.piyadeler.saldiri)) {
    toplam_saldiri_piyadeler -= toplam_saldiri(olum_piyadeler, imparatorluk.unit_insan.piyadeler.saldiri);
} else {
    toplam_saldiri_piyadeler = 0;
}

// Okçular için saldırı toplamını güncelle
if (toplam_saldiri_okcular > toplam_saldiri(olum_okcular, imparatorluk.unit_insan.okcular.saldiri)) {
    toplam_saldiri_okcular -= toplam_saldiri(olum_okcular, imparatorluk.unit_insan.okcular.saldiri);
} else {
    toplam_saldiri_okcular = 0;
}

// Süvariler için saldırı toplamını güncelle
if (toplam_saldiri_suvariler > toplam_saldiri(olum_suvariler, imparatorluk.unit_insan.suvariler.saldiri)) {
    toplam_saldiri_suvariler -= toplam_saldiri(olum_suvariler, imparatorluk.unit_insan.suvariler.saldiri);
} else {
    toplam_saldiri_suvariler = 0;
}

// Kuşatma makineleri için saldırı toplamını güncelle
if (toplam_saldiri_kusatma_makineleri > toplam_saldiri(olum_kusatma_makineleri, imparatorluk.unit_insan.kusatma_makineleri.saldiri)) {
    toplam_saldiri_kusatma_makineleri -= toplam_saldiri(olum_kusatma_makineleri, imparatorluk.unit_insan.kusatma_makineleri.saldiri);
} else {
    toplam_saldiri_kusatma_makineleri = 0;
}

if (toplam_savunma_okcular > toplam_savunma(olum_okcular, imparatorluk.unit_insan.okcular.savunma)) {
    toplam_savunma_okcular -= toplam_savunma(olum_okcular, imparatorluk.unit_insan.okcular.savunma);
} else {
    toplam_savunma_okcular = 0;
}

// Piyadeler için savunma toplamını güncelle
if (toplam_savunma_piyadeler > toplam_savunma(olum_piyadeler, imparatorluk.unit_insan.piyadeler.savunma)) {
    toplam_savunma_piyadeler -= toplam_savunma(olum_piyadeler, imparatorluk.unit_insan.piyadeler.savunma);
} else {
    toplam_savunma_piyadeler = 0;
}

// Süvariler için savunma toplamını güncelle
if (toplam_savunma_suvariler > toplam_savunma(olum_suvariler, imparatorluk.unit_insan.suvariler.savunma)) {
    toplam_savunma_suvariler -= toplam_savunma(olum_suvariler, imparatorluk.unit_insan.suvariler.savunma);
} else {
    toplam_savunma_suvariler = 0;
}

// Kuşatma makineleri için savunma toplamını güncelle
if (toplam_savunma_kusatma_makineleri > toplam_savunma(olum_kusatma_makineleri, imparatorluk.unit_insan.kusatma_makineleri.savunma)) {
    toplam_savunma_kusatma_makineleri -= toplam_savunma(olum_kusatma_makineleri, imparatorluk.unit_insan.kusatma_makineleri.savunma);
} else {
    toplam_savunma_kusatma_makineleri = 0;
}

            // Ekrana yazdıralım
toplam_saldiri_insan=toplam_saldiri_piyadeler+toplam_saldiri_okcular+toplam_saldiri_suvariler+toplam_saldiri_kusatma_makineleri;
 toplam_saldiri_ork=toplam_saldiri_ork_dovusculeri+toplam_saldiri_mizrakcilar+toplam_saldiri_varg_biniciler+toplam_saldiri_troller;
 toplam_savunma_insan = toplam_savunma_piyadeler + toplam_savunma_okcular + toplam_savunma_suvariler + toplam_savunma_kusatma_makineleri;
toplam_savunma_ork = toplam_savunma_ork_dovusculeri + toplam_savunma_mizrakcilar + toplam_savunma_varg_biniciler + toplam_savunma_troller;


   toplam_saglik_insan=imparatorluk.unit_insan.kusatma_makineleri.saglik+imparatorluk.unit_insan.okcular.saglik+imparatorluk.unit_insan.piyadeler.saglik+imparatorluk.unit_insan.suvariler.saglik;
            // İnsan sayısını güncelle
            insan_sayisi = insan_imparatorlugu.insan_birimleri.piyadeler +
                           insan_imparatorlugu.insan_birimleri.okcular +
                           insan_imparatorlugu.insan_birimleri.suvariler +
                           insan_imparatorlugu.insan_birimleri.kusatma_makineleri;

      adim_saldiri_ork++;
printf("--------------------------\n");

}
    adim++;
    }
yuzde_saglik_okcular = (imparatorluk.unit_insan.okcular.saglik / saglik_okcular) * 100.0;
yuzde_saglik_piyadeler = (imparatorluk.unit_insan.piyadeler.saglik / saglik_piyadeler) * 100.0;
yuzde_saglik_kusatma_makineleri = (imparatorluk.unit_insan.kusatma_makineleri.saglik / saglik_kusatma_makineleri) * 100.0;
yuzde_saglik_suvariler = (imparatorluk.unit_insan.suvariler.saglik / saglik_suvariler) * 100.0;
yuzde_saglik_ork_dovusculeri = (imparatorluk.unit_org.ork_dovusculeri.saglik / saglik_ork_dovusculeri) * 100.0;
yuzde_saglik_mizrakcilar = (imparatorluk.unit_org.mizrakcilar.saglik / saglik_mizrakcilar) * 100.0;
yuzde_saglik_varg_biniciler = (imparatorluk.unit_org.varg_binicileri.saglik / saglik_varg_biniciler) * 100.0;
yuzde_saglik_troller = (imparatorluk.unit_org.troller.saglik / saglik_troller) * 100.0;

InitWindow(screenWidth, screenHeight, "Oyun sonu");
SetTargetFPS(60);  // FPS'i 60 olarak ayarla

// Görselleri yükleme
Texture2D textures1[] = {
    LoadTexture("okcular.png"),        // 0. indeks: Okçular
    LoadTexture("piyadeler.png"),      // 1. indeks: Piyadeler
    LoadTexture("suvariler.png"),      // 2. indeks: Süvariler
    LoadTexture("kusatma_makineleri.png"), // 3. indeks: Kuşatma makineleri
    LoadTexture("ork_dovusculeri.png"),// 4. indeks: Ork dövüşçüleri
    LoadTexture("varg_binicileri.png"),// 5. indeks: Varg binicileri
    LoadTexture("troller.png"),        // 6. indeks: Troller
    LoadTexture("mizrakcilar.png")     // 7. indeks: Mızrakçılar
};
// Oyun döngüsü
while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);  // Arka planı beyaz yap

    // Izgarayı çiz
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            DrawRectangleLines(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK);
        }
    }
    // İnsan birimlerini yerleştir
    BirimleriYerlestir(textures1[0], insan_imparatorlugu.insan_birimleri.okcular, yuzde_saglik_okcular, 0, 0); // Okçular
    BirimleriYerlestir(textures1[1], insan_imparatorlugu.insan_birimleri.piyadeler, yuzde_saglik_piyadeler, 2, 0); // Piyadeler
    BirimleriYerlestir(textures1[2], insan_imparatorlugu.insan_birimleri.suvariler, yuzde_saglik_suvariler, 4, 0); // Süvariler
    BirimleriYerlestir(textures1[3], insan_imparatorlugu.insan_birimleri.kusatma_makineleri, yuzde_saglik_kusatma_makineleri, 6, 0); // Kuşatma makineleri

    // Ork birimlerini yerleştir
    BirimleriYerlestir(textures1[4], ork_legi.ork_birimleri.ork_dovusculeri, yuzde_saglik_ork_dovusculeri, 12, 0); // Ork dövüşçüleri
    BirimleriYerlestir(textures1[5], ork_legi.ork_birimleri.varg_binicileri, yuzde_saglik_varg_biniciler, 14, 0); // Varg binicileri
    BirimleriYerlestir(textures1[6], ork_legi.ork_birimleri.troller, yuzde_saglik_troller, 16, 0); // Troller
    BirimleriYerlestir(textures1[7], ork_legi.ork_birimleri.mizrakcilar, yuzde_saglik_mizrakcilar, 18, 0); // Mızrakçılar

    EndDrawing();
}

// Kaynakları serbest bırakıyoruz
for (int i = 0; i < 8; i++) {
    UnloadTexture(textures1[i]);
}


  // Savaşın sonucunu belirt
    if (insan_sayisi <= 0||toplam_saglik_insan<=0) {

        printf("Orklar savasi kazandi!\n");
        fprintf(savas_raporu,"Orklar savasi kazandi!\n");
        printf("Ork sayisi: %d\nOrk toplam saglik %.f\n",ork_sayisi,toplam_saglik_ork);
        fprintf(savas_raporu, "Ork sayisi:%d\nOrk toplam saglik:%.f\n", ork_sayisi,toplam_saglik_ork);

    } else if (ork_sayisi <= 0||toplam_saglik_ork<=0) {

        printf("Insanlar savasi kazandi!\n");
        fprintf(savas_raporu,"Insanlar savasi kazandi!\n");
        printf("Insan sayisi: %d\nInsan toplam saglik %.f\n",insan_sayisi,toplam_saglik_insan);
        fprintf(savas_raporu,"Insan sayisi: %d\nInsan  toplam saglik: %.f\n",insan_sayisi,toplam_saglik_insan);
    }
// Dosyayı kapatma
    fclose(savas_raporu);
    printf("\nSavas raporu 'savas.sim.txt' dosyasina kaydedildi.\n");



  exit(0);
    return 0;
}

