#define _CRT_SECURE_NO_WARNINGS
#include <iostream>     // Para entrada e saída
#include <vector>       // Para criar buffers de imagem
#include <shlobj.h>     // Para obter o caminho da pasta Downloads
#include "QrCode.hpp"   // Biblioteca para gerar QR Codes

// Define a implementação da stb_image_write
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;
using qrcodegen::QrCode;

// Função para obter o caminho da pasta Downloads
std::string GetDownloadsFolderPath() {
    // Obter o caminho da pasta de downloads
    PWSTR path;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &path))) {
        std::wstring wstr(path);
        std::string str(wstr.begin(), wstr.end());
        CoTaskMemFree(path);
        return str + "\\";
    }
    return "";
}

void saveQrCodeAsPng(const QrCode& qr, const string& filename, int scale) {
    int size = qr.getSize();
    int imageSize = size * scale;

    // Cria um buffer para a imagem em escala de cinza (0 = preto, 255 = branco)
    vector<uint8_t> image(imageSize * imageSize, 255);

    // Desenha o QR Code no buffer
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qr.getModule(x, y)) {
                for (int dy = 0; dy < scale; dy++) {
                    for (int dx = 0; dx < scale; dx++) {
                        int px = (y * scale + dy) * imageSize + (x * scale + dx);
                        image[px] = 0; // Preto
                    }
                }
            }
        }
    }

    // Obtém o caminho da pasta Downloads
    std::string downloadPath = GetDownloadsFolderPath() + filename;

    // Verifica se o caminho é válido antes de tentar salvar o arquivo
    if (downloadPath.empty()) {
        cerr << "Erro ao obter o caminho da pasta Downloads!" << endl;
        return;
    }

    // Salva o buffer como PNG
    if (stbi_write_png(downloadPath.c_str(), imageSize, imageSize, 1, image.data(), imageSize) != 0) {
        cout << "QR Code salvo como '" << filename << "' com sucesso!" << endl;
    }
    else {
        cerr << "Erro ao salvar o QR Code como PNG!" << endl;
    }
}

int main() {
    // Passo 1: Obter o texto do usuário
    string text;
    cout << "Digite o texto ou link para gerar o QR Code: ";
    getline(cin, text);

    // Passo 2: Gerar o QR Code
    QrCode qr = QrCode::encodeText(text.c_str(), QrCode::Ecc::MEDIUM);

    // Passo 3: Configuração da saída
    cout << "QR Code gerado (impressão no console):\n";
    int size = qr.getSize();
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            cout << (qr.getModule(x, y) ? "##" : "  ");
        }
        cout << endl;
    }

    // Passo 4: Salvar o QR Code em PNG
    saveQrCodeAsPng(qr, "qrcode.png", 10);

    return 0;
}
