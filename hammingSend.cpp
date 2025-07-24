#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

class HammingSender {
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
        
        cout << "Datos originales: " << data << endl;
        cout << "Bits de datos (m): " << m << endl;
        cout << "Bits de paridad necesarios (r): " << r << endl;
        cout << "Longitud total (n): " << n << endl;
        
        vector<int> hammingCode(n + 1, 0);
        int dataIndex = 0;
        for (int i = 1; i <= n; i++) {
            if (!isPowerOf2(i)) {
                hammingCode[i] = data[dataIndex] - '0';
                dataIndex++;
            }
        }
        
        cout << "Después de insertar datos:" << endl;
        cout << "Pos: ";
        for (int i = 1; i <= n; i++) {
            cout << i << " ";
        }
        cout << endl;
        cout << "Val: ";
        for (int i = 1; i <= n; i++) {
            cout << hammingCode[i] << " ";
        }
        cout << endl;
        for (int i = 0; i < r; i++) {
            int parityPos = pow(2, i);
            int parity = 0;
            
            cout << "Calculando P" << parityPos << " (posición " << parityPos << "):" << endl;
            cout << "Verifica posiciones: ";
            
            for (int j = 1; j <= n; j++) {
                if ((j & parityPos) != 0 && j != parityPos) {
                    parity ^= hammingCode[j];
                    cout << j << " ";
                }
            }
            cout << " -> XOR = " << parity << endl;
            
            hammingCode[parityPos] = parity;
        }
        
        string result = "";
        for (int i = 1; i <= n; i++) {
            result += to_string(hammingCode[i]);
        }
        
        cout << "\nCódigo Hamming final:" << endl;
        cout << "Pos: ";
        for (int i = 1; i <= n; i++) {
            cout << i << " ";
        }
        cout << endl;
        cout << "Val: ";
        for (int i = 1; i <= n; i++) {
            cout << hammingCode[i] << " ";
        }
        cout << endl;
        cout << "Tipo: ";
        for (int i = 1; i <= n; i++) {
            if (isPowerOf2(i)) {
                cout << "P ";
            } else {
                cout << "D ";
            }
        }
        cout << endl;
        
        return result;
    }
};

int main() {
    HammingSender sender;
    string input;
    
    cout << "=== EMISOR CÓDIGO DE HAMMING ===" << endl;
    cout << "Ingrese la trama en binario: ";
    cin >> input;
    
    for (char c : input) {
        if (c != '0' && c != '1') {
            cout << "Error: La entrada debe contener solo 0s y 1s" << endl;
            return 1;
        }
    }
    
    string encoded = sender.encodeHamming(input);
    cout << "\nOutput final: " << encoded << endl;
    
    return 0;
}