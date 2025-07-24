import math

class HammingReceiver:
    def __init__(self):
        pass
    
    def is_power_of_2(self, n):
        return n > 0 and (n & (n - 1)) == 0
    
    def calculate_parity_bits(self, total_bits):
        r = 1
        while 2**r < total_bits + 1:
            r += 1
        return r - 1
    
    def decode_hamming(self, received_data):
        print(f"=== RECEPTOR CÓDIGO DE HAMMING ===")
        print(f"Datos recibidos: {received_data}")
        
        n = len(received_data)
        r = self.calculate_parity_bits(n)
        
        print(f"Longitud total recibida (n): {n}")
        print(f"Bits de paridad esperados (r): {r}")
        
        hamming_code = [0] + [int(bit) for bit in received_data]
        
        print("\nEstructura recibida:")
        print("Pos:", end=" ")
        for i in range(1, n + 1):
            print(f"{i:2}", end=" ")
        print()
        print("Val:", end=" ")
        for i in range(1, n + 1):
            print(f"{hamming_code[i]:2}", end=" ")
        print()
        print("Tipo:", end=" ")
        for i in range(1, n + 1):
            print("P " if self.is_power_of_2(i) else "D ", end=" ")
        print()
        
        error_syndrome = 0
        
        print("\nVerificación de bits de paridad:")
        for i in range(r):
            parity_pos = 2**i
            calculated_parity = 0
            
            controlled_positions = []
            for j in range(1, n + 1):
                if (j & parity_pos) != 0 and j != parity_pos:
                    calculated_parity ^= hamming_code[j]
                    controlled_positions.append(j)
            
            received_parity = hamming_code[parity_pos]
            
            print(f"P{parity_pos} (pos {parity_pos}):")
            print(f"  Posiciones controladas: {controlled_positions}")
            print(f"  Valores: {[hamming_code[pos] for pos in controlled_positions]}")
            print(f"  Paridad recibida: {received_parity}")
            print(f"  Paridad calculada: {calculated_parity}")
            
            if received_parity != calculated_parity:
                error_syndrome += parity_pos
                print(f"¡DIFERENCIA! Agregando {parity_pos} al síndrome")
            else:
                print(f"Coinciden")
        
        print(f"\nSíndrome de error: {error_syndrome}")
        
        if error_syndrome == 0:
            print("✓ No se detectaron errores")
            
            original_data = ""
            print("\nExtrayendo datos originales:")
            for i in range(1, n + 1):
                if not self.is_power_of_2(i):
                    original_data += str(hamming_code[i])
                    print(f"Posición {i}: {hamming_code[i]} (dato)")
            
            print(f"Datos originales extraídos: {original_data}")
            return {
                'status': 'no_error',
                'original_data': original_data,
                'corrected_position': None
            }
        
        else:
            if error_syndrome <= n:
                print(f"✓ Error detectado en la posición {error_syndrome}")
                print(f"Bit erróneo: {hamming_code[error_syndrome]} -> {1 - hamming_code[error_syndrome]}")
                
                hamming_code[error_syndrome] = 1 - hamming_code[error_syndrome]
                print(f"Código corregido:")
                print("Pos:", end=" ")
                for i in range(1, n + 1):
                    print(f"{i:2}", end=" ")
                print()
                print("Val:", end=" ")
                for i in range(1, n + 1):
                    print(f"{hamming_code[i]:2}", end=" ")
                print()
                
                original_data = ""
                print("\nExtrayendo datos corregidos:")
                for i in range(1, n + 1):
                    if not self.is_power_of_2(i):
                        original_data += str(hamming_code[i])
                        print(f"Posición {i}: {hamming_code[i]} (dato)")
                
                print(f"Datos originales recuperados: {original_data}")
                
                return {
                    'status': 'corrected',
                    'original_data': original_data,
                    'corrected_position': error_syndrome,
                    'corrected_data': ''.join(str(hamming_code[i]) for i in range(1, n + 1))
                }
            else:
                print(f"Error detectado pero no se puede corregir (síndrome: {error_syndrome})")
                print("Posiblemente hay múltiples errores o el código está corrupto")
                
                return {
                    'status': 'uncorrectable_error',
                    'original_data': None,
                    'corrected_position': None
                }

def main():
    receiver = HammingReceiver()
    
    print("Ingrese los datos recibidos (código Hamming): ", end="")
    received_data = input().strip()
    
    if not all(bit in '01' for bit in received_data):
        print("Error: La entrada debe contener solo 0s y 1s")
        return
    
    if len(received_data) == 0:
        print("Error: La entrada no puede estar vacía")
        return
    
    result = receiver.decode_hamming(received_data)
    
    print(f"\n=== RESULTADO FINAL ===")
    if result['status'] == 'no_error':
        print(f"Estado: Sin errores detectados")
        print(f"Mensaje original: {result['original_data']}")
    elif result['status'] == 'corrected':
        print(f"Estado: Error detectado y corregido")
        print(f"Posición corregida: {result['corrected_position']}")
        print(f"Mensaje original: {result['original_data']}")
    else:
        print(f"Estado: Error no corregible detectado")
        print(f"La trama se descarta")

if __name__ == "__main__":
    main()