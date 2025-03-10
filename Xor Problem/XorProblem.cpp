#include <iostream>
#include <cmath>
#include <fstream>
#include <ctime>

using namespace std;

// Sigmoid fonksiyonu
double sigmoid(double deger) {
    return 1 / (1 + exp(-deger));
}

// Sigmoid fonksiyonunun türevi
double sigmoidTurevi(double deger) {
    return deger * (1 - deger);
}

// Ağırlıkları ve biasları güncelleme fonksiyonu
void agirlikVeBiasGuncelle(double &parametre, double giris, double hata, double ogrenmeOrani) {
    parametre += ogrenmeOrani * hata * giris;
}

// İleri yayılım fonksiyonu
void ileriYayilim(double girisler[], double &h1, double &h2, double &cikti, double w[], double b[]) {
    h1 = sigmoid(w[0] * girisler[0] + w[1] * girisler[1] + b[0]);
    h2 = sigmoid(w[2] * girisler[0] + w[3] * girisler[1] + b[1]);
    cikti = sigmoid(w[4] * h1 + w[5] * h2 + b[2]);
}

// Geri yayılım fonksiyonu
void geriYayilim(double girisler[], double beklenenCikti, double &h1, double &h2, double &cikti, double w[], double b[], double ogrenmeOrani) {
    // İleri besleme
    double g1, g2, y;
    ileriYayilim(girisler, g1, g2, y, w, b);

    // Hata hesaplama
    double hata = beklenenCikti - cikti;

    // Çıktı katmanı için geri yayılım
    double ciktiTurevi = hata * sigmoidTurevi(cikti);
    agirlikVeBiasGuncelle(w[4], h1, ciktiTurevi, ogrenmeOrani);
    agirlikVeBiasGuncelle(w[5], h2, ciktiTurevi, ogrenmeOrani);
    agirlikVeBiasGuncelle(b[2], 1, ciktiTurevi, ogrenmeOrani);

    // Gizli katman için geri yayılım
    double h1Turevi = ciktiTurevi * w[4] * sigmoidTurevi(h1);
    double h2Turevi = ciktiTurevi * w[5] * sigmoidTurevi(h2);
    agirlikVeBiasGuncelle(w[0], girisler[0], h1Turevi, ogrenmeOrani);
    agirlikVeBiasGuncelle(w[1], girisler[1], h1Turevi, ogrenmeOrani);
    agirlikVeBiasGuncelle(w[2], girisler[0], h2Turevi, ogrenmeOrani);
    agirlikVeBiasGuncelle(w[3], girisler[1], h2Turevi, ogrenmeOrani);
    agirlikVeBiasGuncelle(b[0], 1, (h1Turevi + h2Turevi) / 2, ogrenmeOrani); // Ortalama bias güncellemesi
}

int main() {
    srand(time(0)); // Rastgele sayı üreticisini başlat

    // Ağırlıklar ve biaslar
    double w[6]; // Ağırlıkları random al
    double b[3]; // Biasları random al
    for (int i = 0; i < 6; ++i) {
        w[i] = (double)rand() / RAND_MAX; // 0 ile 1 arasında rastgele ağırlık değerleri
    }
    for (int i = 0; i < 3; ++i) {
        b[i] = (double)rand() / RAND_MAX; // 0 ile 1 arasında rastgele bias değerleri
    }
    double ogrenmeOrani = 0.5;

    // Eğitim verileri: XOR girişleri ve beklenen çıktılar
    double girisler[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double beklenenCiktilar[4] = {0, 1, 1, 0};

    // Hata değerlerini saklamak için dizi
    double hatalar[10000]; // 10000 eğitim döngüsü için hata dizisi

    // Eğitim döngüsü
    for (int epoch = 0; epoch < 10000; ++epoch) {
        double toplamHata = 0;
        for (int i = 0; i < 4; ++i) {
            double h1, h2, cikti;
            double giris[2] = {girisler[i][0], girisler[i][1]};
            ileriYayilim(giris, h1, h2, cikti, w, b);
            geriYayilim(giris, beklenenCiktilar[i], h1, h2, cikti, w, b, ogrenmeOrani);
            toplamHata += abs(beklenenCiktilar[i] - cikti);
        }
        hatalar[epoch] = toplamHata / 4; // Ortalama hatayı kaydet
    }

    // Hata verilerini bir dosyaya yaz
    ofstream dataFile("hatalar.dat");
    for (int i = 0; i < 10000; ++i) {
        dataFile << i << " " << hatalar[i] << endl;
    }
    dataFile.close();

    // Gnuplot'u çağır ve grafiği çiz
    string command = "gnuplot -p -e \"set terminal wxt; plot 'hatalar.dat' with lines\"";
    system(command.c_str());

    // Test
    cout << "Test sonuçları:" << endl;
    for (int i = 0; i < 4; ++i) {
        double h1, h2, cikti;
        double giris[2] = {girisler[i][0], girisler[i][1]};
        ileriYayilim(giris, h1, h2, cikti, w, b);
        cout << "Girdi: " << girisler[i][0] << ", " << girisler[i][1];
        cout << " Çıktı: " << cikti << endl;
    }

    return 0;
}