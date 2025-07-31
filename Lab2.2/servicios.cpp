#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <iomanip>
#include <bitset>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


using namespace std;
void enviarTramaPorSocket(const string& trama) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Error al crear socket." << endl;
        return;
    }

    sockaddr_in servidor;
    memset(&servidor, 0, sizeof(servidor));
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(12345); 
    servidor.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    if (connect(sock, (sockaddr*)&servidor, sizeof(servidor)) < 0) {
        cerr << "Error al conectar al receptor." << endl;
        close(sock);
        return;
    }

    send(sock, trama.c_str(), trama.size(), 0);
    cout << "Trama enviada al receptor por socket." << endl;
    close(sock);
}


class HammingAlgorithm {
private:
    int calculateParityBits(int dataBits) {
        int r = 1;
        while (pow(2, r) < dataBits + r + 1) {
            r++;
        }
        return r;
    }
    
    bool isPowerOf2(int n) {
        return n > 0 && (n & (n - 1)) == 0;
    }

public:
    string encodeHamming(string data) {
        int m = data.length();
        int r = calculateParityBits(m);
        int n = m + r;
        
        cout << "  Aplicando código de Hamming:" << endl;
        cout << "  - Bits de datos: " << m << endl;
        cout << "  - Bits de paridad necesarios: " << r << endl;
        cout << "  - Longitud total: " << n << endl;
        
        vector<int> hammingCode(n + 1, 0);
        int dataIndex = 0;
        
        for (int i = 1; i <= n; i++) {
            if (!isPowerOf2(i)) {
                hammingCode[i] = data[dataIndex] - '0';
                dataIndex++;
            }
        }
        
        for (int i = 0; i < r; i++) {
            int parityPos = pow(2, i);
            int parity = 0;
            
            for (int j = 1; j <= n; j++) {
                if ((j & parityPos) != 0 && j != parityPos) {
                    parity ^= hammingCode[j];
                }
            }
            
            hammingCode[parityPos] = parity;
        }
        
        string result = "";
        for (int i = 1; i <= n; i++) {
            result += to_string(hammingCode[i]);
        }
        
        cout << "  - Código con paridad: " << result << endl;
        return result;
    }
    
    pair<bool, string> verifyAndCorrect(string received) {
        int n = received.length();
        vector<int> code(n + 1, 0);
        
        for (int i = 1; i <= n; i++) {
            code[i] = received[i-1] - '0';
        }
        
        int syndrome = 0;
        int r = 0;
        while (pow(2, r) <= n) {
            int parityPos = pow(2, r);
            int parity = 0;
            
            for (int j = 1; j <= n; j++) {
                if ((j & parityPos) != 0) {
                    parity ^= code[j];
                }
            }
            
            if (parity != 0) {
                syndrome += parityPos;
            }
            r++;
        }
        
        string corrected = received;
        bool hasError = (syndrome != 0);
        
        if (hasError) {
            cout << "Error detectado - Síndrome: " << syndrome << endl;
            if (syndrome > 0 && syndrome <= n) {
                code[syndrome] = 1 - code[syndrome];
                corrected = "";
                for (int i = 1; i <= n; i++) {
                    corrected += to_string(code[i]);
                }
                cout << "Error simple corregido en posición " << syndrome << endl;
                cout << "Trama corregida: " << corrected << endl;
            } else if (syndrome > n) {
                cout << "Errores múltiples detectados" << endl;
                cout << "Hamming solo puede corregir errores simples" << endl;
                cout << "Trama no corregida debido a errores múltiples" << endl;
            }
        } else {
            cout << "No se detectaron errores" << endl;
        }
        
        return make_pair(hasError, corrected);
    }
};

class ApplicationLayer {
public:
    pair<string, string> solicitarMensaje() {
        string mensaje, algoritmo;
        
        cout << "\n=== CAPA DE APLICACIÓN ===" << endl;
        cout << "Ingrese el mensaje a enviar: ";
        getline(cin, mensaje);
        
        cout << "Algoritmos disponibles:" << endl;
        cout << "1. hamming" << endl;
        cout << "Seleccione el algoritmo de integridad: ";
        getline(cin, algoritmo);
        
        cout << "[APLICACIÓN] Mensaje: \"" << mensaje << "\"" << endl;
        cout << "[APLICACIÓN] Algoritmo seleccionado: " << algoritmo << endl;
        
        return make_pair(mensaje, algoritmo);
    }
};

class PresentationLayer {
public:
    string codificarMensaje(const string& mensaje) {
        cout << "Codificando mensaje en ASCII binario..." << endl;
        
        string mensajeBinario = "";
        
        for (int i = 0; i < mensaje.length(); i++) {
            char c = mensaje[i];
            bitset<8> binario(c);
            string charBinario = binario.to_string();
            mensajeBinario += charBinario;
            
            cout << "Mensaje'" << c << "' (ASCII " << (int)c << ") -> " << charBinario << endl;
        }
        
        cout << "Mensaje completo en binario: " << mensajeBinario << endl;
        return mensajeBinario;
    }
};

class DataLinkLayer {
private:
    HammingAlgorithm hamming;
    
public:
    string calcularIntegridad(const string& datos, const string& algoritmo) {
        
        if (algoritmo == "hamming") {
            cout << "Aplicando algoritmo de Hamming..." << endl;
            string resultado = "";
            for (size_t i = 0; i < datos.length(); i += 8) {
                string bloque = datos.substr(i, 8);
                resultado += hamming.encodeHamming(bloque);
            }
            return resultado;

        } else {
            cout << "Algoritmo no reconocido, usando datos sin modificar" << endl;
            return datos;
        }
    }
    
    pair<bool, string> verificarIntegridad(const string& datosRecibidos, const string& algoritmo) {
        cout << "\nVerificando integridad..." << endl;
        
        if (algoritmo == "hamming") {
            return hamming.verifyAndCorrect(datosRecibidos);
        } else {
            return make_pair(false, datosRecibidos);
        }
    }
};

class NoiseSimulator {
private:
    random_device rd;
    mt19937 gen;
    
public:
    NoiseSimulator() : gen(rd()) {}
    
    string aplicarRuido(const string& trama, double tasaError) {
        cout << "\n=== SIMULADOR DE RUIDO ===" << endl;
        cout << "Aplicando ruido con tasa de error: " << tasaError << endl;
        cout << "Trama original: " << trama << endl;
        
        string tramaConRuido = trama;
        uniform_real_distribution<> dis(0.0, 1.0);
        int erroresAplicados = 0;
        
        for (int i = 0; i < tramaConRuido.length(); i++) {
            if (dis(gen) < tasaError) {
                tramaConRuido[i] = (tramaConRuido[i] == '0') ? '1' : '0';
                erroresAplicados++;
                cout << "Bit flip en posición " << (i + 1) << endl;
            }
        }
        
        cout << "Total de errores aplicados: " << erroresAplicados << endl;
        cout << "Trama con ruido: " << tramaConRuido << endl;
        
        return tramaConRuido;
    }
    
    double solicitarTasaError() {
        double tasa;
        cout << "\nIngrese la tasa de error (Ponga 0.01 para 1% de error): ";
        cin >> tasa;
        return tasa;
    }
};

class CommunicationSystem {
private:
    ApplicationLayer app;
    PresentationLayer presentation;
    DataLinkLayer dataLink;
    NoiseSimulator noise;
    
public:
    void ejecutarEmisor() {
        cout << "=== SISTEMA DE COMUNICACIÓN - EMISOR ===" << endl;
        
        pair<string, string> respuestaApp = app.solicitarMensaje();
        string mensaje = respuestaApp.first;
        string algoritmo = respuestaApp.second;
        
        string mensajeBinario = presentation.codificarMensaje(mensaje);
        
        string tramaConIntegridad = dataLink.calcularIntegridad(mensajeBinario, algoritmo);
        
        double tasaError = noise.solicitarTasaError();
        string tramaConRuido = noise.aplicarRuido(tramaConIntegridad, tasaError);
        enviarTramaPorSocket(tramaConRuido);

        
        pair<bool, string> resultado = dataLink.verificarIntegridad(tramaConRuido, algoritmo);
        bool tieneError = resultado.first;
        string tramaCoorregida = resultado.second;
        
        cout << "\n=== RESUMEN DE LA TRANSMISIÓN ===" << endl;
        cout << "Mensaje original: \"" << mensaje << "\"" << endl;
        cout << "Mensaje en binario: " << mensajeBinario << endl;
        cout << "Con bits de integridad: " << tramaConIntegridad << endl;
        cout << "Después del ruido: " << tramaConRuido << endl;
        cout << "Después de corrección: " << tramaCoorregida << endl;
        cout << "¿Se detectaron errores?: " << (tieneError ? "SÍ" : "NO") << endl;
    }
};

int main() {
    CommunicationSystem sistema;
    sistema.ejecutarEmisor();
    return 0;
}