import argparse
import csv
import time
from datetime import datetime
from pathlib import Path

import matplotlib.pyplot as plt


BAUDIOS = 9600
DURACION_PREDETERMINADA = 120


def leer_argumentos():
    parser = argparse.ArgumentParser(
        description="Registra y grafica los datos del control de temperatura."
    )
    parser.add_argument(
        "--puerto",
        default="COM3",
        help="Puerto serial del Arduino. Valor inicial: COM3.",
    )
    parser.add_argument(
        "--duracion",
        type=int,
        default=DURACION_PREDETERMINADA,
        help="Duracion de la medicion en segundos. Valor inicial: 120.",
    )
    parser.add_argument(
        "--archivo",
        type=Path,
        help="Grafica un CSV existente en lugar de leer el puerto serial.",
    )
    return parser.parse_args()


def interpretar_linea(linea, tiempo_segundos):
    if not linea.startswith("DATO,"):
        return None

    partes = linea.split(",")
    if len(partes) != 6:
        return None

    try:
        return {
            "tiempo_s": round(tiempo_segundos, 1),
            "temperatura_c": int(partes[1]),
            "punto_medio_c": int(partes[2]),
            "calefactor": int(partes[3]),
            "ventilador_pct": int(partes[4]),
            "estado": partes[5].strip(),
        }
    except ValueError:
        return None


def registrar_desde_serial(puerto, duracion):
    try:
        import serial
    except ImportError as error:
        raise RuntimeError(
            "Falta pyserial. Ejecutar: py -m pip install -r requirements.txt"
        ) from error

    datos = []
    print(f"Abriendo {puerto} a {BAUDIOS} baudios...")

    with serial.Serial(puerto, BAUDIOS, timeout=1) as conexion:
        time.sleep(2)
        conexion.reset_input_buffer()
        inicio = time.monotonic()

        print(f"Registrando durante {duracion} segundos. Presionar Ctrl+C para terminar.")
        try:
            while time.monotonic() - inicio < duracion:
                linea = conexion.readline().decode("ascii", errors="ignore").strip()
                dato = interpretar_linea(linea, time.monotonic() - inicio)

                if dato is not None:
                    datos.append(dato)
                    print(
                        f"{dato['tiempo_s']:6.1f} s | "
                        f"{dato['temperatura_c']:3d} C | "
                        f"PM {dato['punto_medio_c']:3d} C | "
                        f"{dato['estado']}"
                    )
        except KeyboardInterrupt:
            print("\nRegistro detenido por el usuario.")

    return datos


def cargar_csv(ruta):
    datos = []
    with ruta.open("r", encoding="utf-8", newline="") as archivo:
        lector = csv.DictReader(archivo)
        for fila in lector:
            datos.append(
                {
                    "tiempo_s": float(fila["tiempo_s"]),
                    "temperatura_c": int(fila["temperatura_c"]),
                    "punto_medio_c": int(fila["punto_medio_c"]),
                    "calefactor": int(fila["calefactor"]),
                    "ventilador_pct": int(fila["ventilador_pct"]),
                    "estado": fila["estado"],
                }
            )
    return datos


def guardar_csv(datos, ruta):
    columnas = [
        "tiempo_s",
        "temperatura_c",
        "punto_medio_c",
        "calefactor",
        "ventilador_pct",
        "estado",
    ]

    with ruta.open("w", encoding="utf-8", newline="") as archivo:
        escritor = csv.DictWriter(archivo, fieldnames=columnas)
        escritor.writeheader()
        escritor.writerows(datos)


def crear_grafica(datos, ruta):
    tiempos = [dato["tiempo_s"] for dato in datos]
    temperaturas = [dato["temperatura_c"] for dato in datos]
    puntos_medios = [dato["punto_medio_c"] for dato in datos]
    limites_inferiores = [max(0, punto - 5) for punto in puntos_medios]
    limites_superiores = [min(100, punto + 6) for punto in puntos_medios]
    calefactor_pct = [dato["calefactor"] * 100 for dato in datos]
    ventilador_pct = [dato["ventilador_pct"] for dato in datos]

    figura, (grafica_temp, grafica_acciones) = plt.subplots(
        2, 1, figsize=(11, 7), sharex=True, layout="constrained"
    )

    grafica_temp.fill_between(
        tiempos,
        limites_inferiores,
        limites_superiores,
        color="#8fd19e",
        alpha=0.35,
        label="Rango ideal",
    )
    grafica_temp.plot(
        tiempos, temperaturas, color="#c62828", marker="o", label="Temperatura"
    )
    grafica_temp.plot(
        tiempos, puntos_medios, color="#333333", linestyle="--", label="Punto medio"
    )
    grafica_temp.set_ylabel("Temperatura (C)")
    grafica_temp.set_title("Comportamiento del control de temperatura")
    grafica_temp.grid(True, alpha=0.3)
    grafica_temp.legend(loc="best")

    grafica_acciones.step(
        tiempos,
        calefactor_pct,
        where="post",
        color="#d97706",
        linewidth=2,
        label="Calefactor (0 o 100 %)",
    )
    grafica_acciones.step(
        tiempos,
        ventilador_pct,
        where="post",
        color="#1565c0",
        linewidth=2,
        label="Velocidad del ventilador",
    )
    grafica_acciones.set_xlabel("Tiempo (s)")
    grafica_acciones.set_ylabel("Activacion (%)")
    grafica_acciones.set_ylim(-5, 105)
    grafica_acciones.set_yticks([0, 35, 65, 100])
    grafica_acciones.grid(True, alpha=0.3)
    grafica_acciones.legend(loc="best")

    figura.savefig(ruta, dpi=160)
    plt.close(figura)


def main():
    argumentos = leer_argumentos()
    carpeta_script = Path(__file__).resolve().parent

    if argumentos.archivo:
        ruta_csv = argumentos.archivo.resolve()
        datos = cargar_csv(ruta_csv)
        ruta_grafica = ruta_csv.with_suffix(".png")
    else:
        datos = registrar_desde_serial(argumentos.puerto, argumentos.duracion)
        if not datos:
            raise RuntimeError(
                "No se recibieron lineas DATO. Revisar el puerto, el baud rate y el programa."
            )

        carpeta_resultados = carpeta_script / "resultados"
        carpeta_resultados.mkdir(exist_ok=True)
        nombre = datetime.now().strftime("prueba_%Y%m%d_%H%M%S")
        ruta_csv = carpeta_resultados / f"{nombre}.csv"
        ruta_grafica = carpeta_resultados / f"{nombre}.png"
        guardar_csv(datos, ruta_csv)

    if not datos:
        raise RuntimeError("El archivo no contiene mediciones.")

    crear_grafica(datos, ruta_grafica)
    print(f"CSV: {ruta_csv}")
    print(f"Grafica: {ruta_grafica}")


if __name__ == "__main__":
    main()
