import socket
from typing import Tuple


def is_power_of_2(n):
    return n != 0 and (n & (n - 1)) == 0

def verificar_y_corregir_hamming(trama: str) -> Tuple[bool, str]:
    n = len(trama)
    code = [0] + [int(b) for b in trama]  
    syndrome = 0
    r = 0
    while (2 ** r) <= n:
        parity_pos = 2 ** r
        parity = 0
        for j in range(1, n + 1):
            if j & parity_pos:
                parity ^= code[j]
        if parity != 0:
            syndrome += parity_pos
        r += 1

    corregida = list(trama)
    if syndrome == 0:
        print("No se detectaron errores en bloque.")
        return False, trama
    elif syndrome <= n:
        print(f"Error detectado en bloque. Corrigiendo en la posición {syndrome}")
        corregida[syndrome - 1] = '0' if corregida[syndrome - 1] == '1' else '1'
        return True, ''.join(corregida)
    else:
        print("Error múltiple detectado en bloque. No se puede corregir.")
        return True, trama


def decodificar_mensaje_hamming(trama: str) -> Tuple[bool, str]:
    mensaje = ""
    bloque_len = 12  
    errores_detectados = False

    for i in range(0, len(trama), bloque_len):
        bloque = trama[i:i + bloque_len]
        if len(bloque) < bloque_len:
            print(f"Bloque incompleto ignorado: {bloque}")
            continue

        error, corregido = verificar_y_corregir_hamming(bloque)
        errores_detectados |= error

        datos = ""
        for pos in range(1, len(corregido) + 1):
            if not is_power_of_2(pos):
                datos += corregido[pos - 1]

        if len(datos) == 8:
            ascii_val = int(datos, 2)
            mensaje += chr(ascii_val)
        else:
            print("Error al reconstruir byte ASCII.")

    return errores_detectados, mensaje


def mostrar_mensaje(trama: str):
    error, mensaje = decodificar_mensaje_hamming(trama)
    if error:
        print(f"El mensaje fue corregido parcialmente. Resultado:")
    else:
        print(f"Mensaje recibido correctamente:")
    print(f": \"{mensaje}\"")


def recibir_trama(puerto: int = 12345):
    print(f"Esperando trama en puerto {puerto}...")
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as servidor:
        servidor.bind(('0.0.0.0', puerto))
        servidor.listen(1)
        conn, addr = servidor.accept()
        with conn:
            print(f"Conexión establecida desde {addr}")
            data = conn.recv(4096)
            trama = data.decode()
            print(f"Trama recibida: {trama}")
            return trama


def receptor():
    print("RECEPTOR")
    trama_recibida = recibir_trama()
    mostrar_mensaje(trama_recibida)

if __name__ == "__main__":
    receptor()
