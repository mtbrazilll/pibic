import subprocess
import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

nome = "tesssste"
time_limite = "300"
nsols = "1"
cpl_abort = "1"
init = "0"
warm_start = "0"
h_emph = "2"
max_age = "1"


# Cria a pasta se ela não existir
if not os.path.exists(f'../resultados/{nome}'):
    os.makedirs(f'../resultados/{nome}')

# Lista de instâncias a serem testadas
instancias1 = [
            "../instancias/discos_variando/d1291.udc -r 1500",
            "../instancias/discos_variando/d1291.udc -r 1000",
            "../instancias/discos_variando/d1291.udc -r 750",
            "../instancias/discos_variando/d1291.udc -r 500",
            "../instancias/discos_variando/rl1889.udc -r 4000",
            "../instancias/discos_variando/rl1889.udc -r 3500",
            "../instancias/discos_variando/rl1889.udc -r 3000",
            "../instancias/discos_variando/rl1889.udc -r 2500",
            "../instancias/discos_variando/u2319.udc -r 2000",
            "../instancias/discos_variando/u2319.udc -r 1700",
            "../instancias/discos_variando/u2319.udc -r 1400",
            "../instancias/discos_variando/u2319.udc -r 1000",
            "../instancias/discos_variando/pcb3038.udc -r 1000",
            "../instancias/discos_variando/pcb3038.udc -r 700",
            "../instancias/discos_variando/pcb3038.udc -r 600",
            "../instancias/discos_variando/pcb3038.udc -r 500"]

instancias2 = [
            "../instancias/discos_variando/d1291.udc -r 1500 -t 76.1",
            "../instancias/discos_variando/d1291.udc -r 1000 -t 76.7",
            "../instancias/discos_variando/d1291.udc -r 750 -t 101.2",
            "../instancias/discos_variando/d1291.udc -r 500 -t 171.3",
            "../instancias/discos_variando/rl1889.udc -r 4000 -t 138.3",
            "../instancias/discos_variando/rl1889.udc -r 3500 -t 109.6",
            "../instancias/discos_variando/rl1889.udc -r 3000 -t 127.0",
            "../instancias/discos_variando/rl1889.udc -r 2500 -t 147.4",
            "../instancias/discos_variando/u2319.udc -r 2000 -t 214.8",
            "../instancias/discos_variando/u2319.udc -r 1700 -t 152.1",
            "../instancias/discos_variando/u2319.udc -r 1400 -t 160.1",
            "../instancias/discos_variando/u2319.udc -r 1000 -t 158.6",
            "../instancias/discos_variando/pcb3038.udc -r 1000 -t 308.2",
            "../instancias/discos_variando/pcb3038.udc -r 700 -t 265.8",
            "../instancias/discos_variando/pcb3038.udc -r 600 -t 247.5",
            "../instancias/discos_variando/pcb3038.udc -r 500 -t 292.8"]




instancias5 = [
            "../instancias/discos_variando/d1291.udc",
            "../instancias/discos_variando/rl1889.udc",
            "../instancias/discos_variando/u2319.udc",
            "../instancias/discos_variando/pcb3038.udc",
            "../instancias/Real-world/mona-lisa100k.udc",
            "../instancias/Real-world/nyctaxi_2.9M.udc",
            "../instancias/Real-world/uber_4.5M.udc",
            "../instancias/Real-world/usa_115K.udc",
            "../instancias/Real-world/wildfires_1.8M.udc",
            "../instancias/Real-world/world_1.9M.udc",
            "../instancias/Real-world/hail2015_10M.udc",
            "../instancias/mcz.txt"]



instancias4 = [
             "../instancias/mcz.txt",
            "../instancias/discos_variando/d1291.udc",
            "../instancias/discos_variando/rl1889.udc",
            "../instancias/discos_variando/u2319.udc",
            "../instancias/discos_variando/pcb3038.udc",
            "../instancias/Real-world/mona-lisa100k.udc",
            "../instancias/Real-world/uber_4.5M.udc",
            "../instancias/Real-world/usa_115K.udc",
            "../instancias/Real-world/wildfires_1.8M.udc",
            "../instancias/Real-world/world_1.9M.udc",
            "../instancias/Real-world/hail2015_10M.udc"
            ]

instancias = [
    "../instancias/instancias_geradas/instancias_500/disco_instancia_1.txt",
    "../instancias/instancias_geradas/instancias_500/disco_instancia_2.txt",
    "../instancias/instancias_geradas/instancias_500/disco_instancia_3.txt",
    "../instancias/instancias_geradas/instancias_500/disco_instancia_4.txt",
    "../instancias/instancias_geradas/instancias_500/disco_instancia_5.txt",
    "../instancias/instancias_geradas/instancias_500/anel_instancia_1.txt",
    "../instancias/instancias_geradas/instancias_500/anel_instancia_2.txt",
    "../instancias/instancias_geradas/instancias_500/anel_instancia_3.txt",
    "../instancias/instancias_geradas/instancias_500/anel_instancia_4.txt",
    "../instancias/instancias_geradas/instancias_500/anel_instancia_5.txt",
    "../instancias/instancias_geradas/instancias_500/quadrado_instancia_1.txt",
    "../instancias/instancias_geradas/instancias_500/quadrado_instancia_2.txt",
    "../instancias/instancias_geradas/instancias_500/quadrado_instancia_3.txt",
    "../instancias/instancias_geradas/instancias_500/quadrado_instancia_4.txt",
    "../instancias/instancias_geradas/instancias_500/quadrado_instancia_5.txt",
]

instancias10 = [
    "../instancias/tsp/d1291.pcd -r 500",
    "../instancias/tsp/d1655.pcd -r 500",
    "../instancias/tsp/d2103.pcd -r 500",
    "../instancias/tsp/dsj1000.pcd -r 500",
    "../instancias/tsp/fl1400.pcd -r 500",
    "../instancias/tsp/fl1577.pcd -r 500",
    "../instancias/tsp/fl3795.pcd -r 500",
    "../instancias/tsp/nrw1379.pcd -r 500",
    "../instancias/tsp/pcb1173.pcd -r 500",
    "../instancias/tsp/pcb3038.pcd -r 500",
    "../instancias/tsp/pr1002.pcd -r 500",
    "../instancias/tsp/pr2392.pcd -r 500",
    "../instancias/tsp/rl1304.pcd -r 500",
    "../instancias/tsp/rl1323.pcd -r 500",
    "../instancias/tsp/rl1889.pcd -r 500",
    "../instancias/tsp/rl5915.pcd -r 500",
    "../instancias/tsp/u1060.pcd -r 500",
    "../instancias/tsp/u1432.pcd -r 500",
    "../instancias/tsp/u1817.pcd -r 500",
    "../instancias/tsp/u2152.pcd -r 500",
    "../instancias/tsp/u2319.pcd -r 500",
    "../instancias/tsp/vm1084.pcd -r 500",
    "../instancias/tsp/vm1748.pcd -r 500",
     "../instancias/tsp/d1291.pcd -r 1000",
    "../instancias/tsp/d1655.pcd -r 1000",
    "../instancias/tsp/d2103.pcd -r 1000",
    "../instancias/tsp/dsj1000.pcd -r 1000",
    "../instancias/tsp/fl1400.pcd -r 1000",
    "../instancias/tsp/fl1577.pcd -r 1000",
    "../instancias/tsp/fl3795.pcd -r 1000",
    "../instancias/tsp/nrw1379.pcd -r 1000",
    "../instancias/tsp/pcb1173.pcd -r 1000",
    "../instancias/tsp/pcb3038.pcd -r 1000",
    "../instancias/tsp/pr1002.pcd -r 1000",
    "../instancias/tsp/pr2392.pcd -r 1000",
    "../instancias/tsp/rl1304.pcd -r 1000",
    "../instancias/tsp/rl1323.pcd -r 1000",
    "../instancias/tsp/rl1889.pcd -r 1000",
    "../instancias/tsp/rl5915.pcd -r 1000",
    "../instancias/tsp/u1060.pcd -r 1000",
    "../instancias/tsp/u1432.pcd -r 1000",
    "../instancias/tsp/u1817.pcd -r 1000",
    "../instancias/tsp/u2152.pcd -r 1000",
    "../instancias/tsp/u2319.pcd -r 1000",
    "../instancias/tsp/vm1084.pcd -r 1000",
    "../instancias/tsp/vm1748.pcd -r 1000",
    "../instancias/tsp/d1291.pcd -r  750",
    "../instancias/tsp/d1655.pcd -r  750",
    "../instancias/tsp/d2103.pcd -r  750",
    "../instancias/tsp/dsj1000.pcd -r  750",
    "../instancias/tsp/fl1400.pcd -r  750",
    "../instancias/tsp/fl1577.pcd -r  750",
    "../instancias/tsp/fl3795.pcd -r  750",
    "../instancias/tsp/nrw1379.pcd -r  750",
    "../instancias/tsp/pcb1173.pcd -r  750",
    "../instancias/tsp/pcb3038.pcd -r  750",
    "../instancias/tsp/pr1002.pcd -r  750",
    "../instancias/tsp/pr2392.pcd -r  750",
    "../instancias/tsp/rl1304.pcd -r  750",
    "../instancias/tsp/rl1323.pcd -r  750",
    "../instancias/tsp/rl1889.pcd -r  750",
    "../instancias/tsp/rl5915.pcd -r  750",
    "../instancias/tsp/u1060.pcd -r  750",
    "../instancias/tsp/u1432.pcd -r  750",
    "../instancias/tsp/u1817.pcd -r  750",
    "../instancias/tsp/u2152.pcd -r  750",
    "../instancias/tsp/u2319.pcd -r  750",
    "../instancias/tsp/vm1084.pcd -r  750",
    "../instancias/tsp/vm1748.pcd -r  750",
    "../instancias/tsp/d1291.pcd -r  1500",
    "../instancias/tsp/d1655.pcd -r  1500",
    "../instancias/tsp/d2103.pcd -r  1500",
    "../instancias/tsp/dsj1000.pcd -r  1500",
    "../instancias/tsp/fl1400.pcd -r  1500",
    "../instancias/tsp/fl1577.pcd -r  1500",
    "../instancias/tsp/fl3795.pcd -r  1500",
    "../instancias/tsp/nrw1379.pcd -r  1500",
    "../instancias/tsp/pcb1173.pcd -r  1500",
    "../instancias/tsp/pcb3038.pcd -r  1500",
    "../instancias/tsp/pr1002.pcd -r  1500",
    "../instancias/tsp/pr2392.pcd -r  1500",
    "../instancias/tsp/rl1304.pcd -r  1500",
    "../instancias/tsp/rl1323.pcd -r  1500",
    "../instancias/tsp/rl1889.pcd -r  1500",
    "../instancias/tsp/rl5915.pcd -r  1500",
    "../instancias/tsp/u1060.pcd -r  1500",
    "../instancias/tsp/u1432.pcd -r  1500",
    "../instancias/tsp/u1817.pcd -r  1500",
    "../instancias/tsp/u2152.pcd -r  1500",
    "../instancias/tsp/u2319.pcd -r  1500",
    "../instancias/tsp/vm1084.pcd -r  1500",
    "../instancias/tsp/vm1748.pcd -r  1500"
    "../instancias/tsp/d1291.pcd -r  2000",
    "../instancias/tsp/d1655.pcd -r  2000",
    "../instancias/tsp/d2103.pcd -r  2000",
    "../instancias/tsp/dsj1000.pcd -r  2000",
    "../instancias/tsp/fl1400.pcd -r  2000",
    "../instancias/tsp/fl1577.pcd -r  2000",
    "../instancias/tsp/fl3795.pcd -r  2000",
    "../instancias/tsp/nrw1379.pcd -r  2000",
    "../instancias/tsp/pcb1173.pcd -r  2000",
    "../instancias/tsp/pcb3038.pcd -r  2000",
    "../instancias/tsp/pr1002.pcd -r  2000",
    "../instancias/tsp/pr2392.pcd -r  2000",
    "../instancias/tsp/rl1304.pcd -r  2000",
    "../instancias/tsp/rl1323.pcd -r  2000",
    "../instancias/tsp/rl1889.pcd -r  2000",
    "../instancias/tsp/rl5915.pcd -r  2000",
    "../instancias/tsp/u1060.pcd -r  2000",
    "../instancias/tsp/u1432.pcd -r  2000",
    "../instancias/tsp/u1817.pcd -r  2000",
    "../instancias/tsp/u2152.pcd -r  2000",
    "../instancias/tsp/u2319.pcd -r  2000",
    "../instancias/tsp/vm1084.pcd -r  2000",
    "../instancias/tsp/vm1748.pcd -r  2000"
    
]

instancias10 = [
    "../instancias/tsp/d1291.pcd -r 500",
    "../instancias/tsp/d1655.pcd -r 500",
    "../instancias/tsp/d2103.pcd -r 500",
    "../instancias/tsp/dsj1000.pcd -r 500",
    "../instancias/tsp/fl1400.pcd -r 500",
    "../instancias/tsp/fl1577.pcd -r 500",
    "../instancias/tsp/fl3795.pcd -r 500",
    "../instancias/tsp/nrw1379.pcd -r 500",
    "../instancias/tsp/pcb1173.pcd -r 500",
    "../instancias/tsp/pcb3038.pcd -r 500",
    "../instancias/tsp/pr1002.pcd -r 500",
    "../instancias/tsp/pr2392.pcd -r 500",
    "../instancias/tsp/rl1304.pcd -r 500",
    "../instancias/tsp/rl1323.pcd -r 500",
    "../instancias/tsp/rl1889.pcd -r 500",
    "../instancias/tsp/rl5915.pcd -r 500",
    "../instancias/tsp/u1060.pcd -r 500",
    "../instancias/tsp/u1432.pcd -r 500",
    "../instancias/tsp/u1817.pcd -r 500",
    "../instancias/tsp/u2152.pcd -r 500",
    "../instancias/tsp/u2319.pcd -r 500",
    "../instancias/tsp/vm1084.pcd -r 500",
    "../instancias/tsp/vm1748.pcd -r 500",
     "../instancias/tsp/d1291.pcd -r 1000",
    "../instancias/tsp/d1655.pcd -r 1000",
    "../instancias/tsp/d2103.pcd -r 1000",
    "../instancias/tsp/dsj1000.pcd -r 1000",
    "../instancias/tsp/fl1400.pcd -r 1000",
    "../instancias/tsp/fl1577.pcd -r 1000",
    "../instancias/tsp/fl3795.pcd -r 1000",
    "../instancias/tsp/nrw1379.pcd -r 1000",
    "../instancias/tsp/pcb1173.pcd -r 1000",
    "../instancias/tsp/pcb3038.pcd -r 1000",
    "../instancias/tsp/pr1002.pcd -r 1000",
    "../instancias/tsp/pr2392.pcd -r 1000",
    "../instancias/tsp/rl1304.pcd -r 1000",
    "../instancias/tsp/rl1323.pcd -r 1000",
    "../instancias/tsp/rl1889.pcd -r 1000",
    "../instancias/tsp/rl5915.pcd -r 1000",
    "../instancias/tsp/u1060.pcd -r 1000",
    "../instancias/tsp/u1432.pcd -r 1000",
    "../instancias/tsp/u1817.pcd -r 1000",
    "../instancias/tsp/u2152.pcd -r 1000",
    "../instancias/tsp/u2319.pcd -r 1000",
    "../instancias/tsp/vm1084.pcd -r 1000",
    "../instancias/tsp/vm1748.pcd -r 1000",
    "../instancias/tsp/d1291.pcd -r  2000",
    "../instancias/tsp/d1655.pcd -r  2000",
    "../instancias/tsp/d2103.pcd -r  2000",
    "../instancias/tsp/dsj1000.pcd -r  2000",
    "../instancias/tsp/fl1400.pcd -r  2000",
    "../instancias/tsp/fl1577.pcd -r  2000",
    "../instancias/tsp/fl3795.pcd -r  2000",
    "../instancias/tsp/nrw1379.pcd -r  2000",
    "../instancias/tsp/pcb1173.pcd -r  2000",
    "../instancias/tsp/pcb3038.pcd -r  2000",
    "../instancias/tsp/pr1002.pcd -r  2000",
    "../instancias/tsp/pr2392.pcd -r  2000",
    "../instancias/tsp/rl1304.pcd -r  2000",
    "../instancias/tsp/rl1323.pcd -r  2000",
    "../instancias/tsp/rl1889.pcd -r  2000",
    "../instancias/tsp/rl5915.pcd -r  2000",
    "../instancias/tsp/u1060.pcd -r  2000",
    "../instancias/tsp/u1432.pcd -r  2000",
    "../instancias/tsp/u1817.pcd -r  2000",
    "../instancias/tsp/u2152.pcd -r  2000",
    "../instancias/tsp/u2319.pcd -r  2000",
    "../instancias/tsp/vm1084.pcd -r  2000",
    "../instancias/tsp/vm1748.pcd -r  2000"
    
]



instancias = [
    "../instancias/tsp/d2103.pcd -r 1000",
    "../instancias/tsp/fl3795.pcd -r 500",
    "../instancias/tsp/nrw1379.pcd -r 1000",
    "../instancias/tsp/pcb1173.pcd -r 1000",
    "../instancias/tsp/pcb3038.pcd -r 500",
    "../instancias/tsp/pr2392.pcd -r 1000",
    "../instancias/tsp/pr2392.pcd -r 2000",
    "../instancias/tsp/rl1889.pcd -r 2000",
    "../instancias/tsp/rl5915.pcd -r 1000",
    "../instancias/tsp/rl5915.pcd -r 2000",
    "../instancias/tsp/rl5915.pcd -r 500",
    "../instancias/tsp/u1432.pcd -r 1000",
    "../instancias/tsp/u2319.pcd -r 500"
]

perde = [
    "../instancias/tsp/fl3795.pcd -r 500",
    "../instancias/tsp/pcb3038.pcd -r 500",
    "../instancias/tsp/u2152.pcd -r 500",
    "../instancias/tsp/d2103.pcd -r 1000",
    "../instancias/tsp/nrw1379.pcd -r 1000",
    "../instancias/tsp/pcb1173.pcd -r 1000",
    "../instancias/tsp/pr2392.pcd -r 1000",
    "../instancias/tsp/rl5915.pcd -r 1000",
    "../instancias/tsp/u1432.pcd -r 1000"   
]

competitivas = [
    "../instancias/tsp/d1291.pcd -r 500",
    "../instancias/tsp/d1655.pcd -r 500",
    "../instancias/tsp/d2103.pcd -r 500",
    "../instancias/tsp/dsj1000.pcd -r 500",
    "../instancias/tsp/fl1400.pcd -r 500",
    "../instancias/tsp/fl1577.pcd -r 500",
    "../instancias/tsp/fl3795.pcd -r 500",
    "../instancias/tsp/nrw1379.pcd -r 500",
    "../instancias/tsp/pcb1173.pcd -r 500",
    "../instancias/tsp/pcb3038.pcd -r 500",
    "../instancias/tsp/pr1002.pcd -r 500",
    "../instancias/tsp/pr2392.pcd -r 500",
    "../instancias/tsp/rl1304.pcd -r 500",
    "../instancias/tsp/rl1323.pcd -r 500",
    "../instancias/tsp/rl1889.pcd -r 500",
    "../instancias/tsp/rl5915.pcd -r 500",
    "../instancias/tsp/u1060.pcd -r 500",
    "../instancias/tsp/u1432.pcd -r 500",
    "../instancias/tsp/u1817.pcd -r 500",
    "../instancias/tsp/u2152.pcd -r 500",
    "../instancias/tsp/u2319.pcd -r 500",
    "../instancias/tsp/vm1084.pcd -r 500",
    "../instancias/tsp/vm1748.pcd -r 500",
     "../instancias/tsp/d1291.pcd -r 1000",
    "../instancias/tsp/d1655.pcd -r 1000",
    "../instancias/tsp/d2103.pcd -r 1000",
    "../instancias/tsp/dsj1000.pcd -r 1000",
    "../instancias/tsp/fl1400.pcd -r 1000",
    "../instancias/tsp/fl1577.pcd -r 1000",
    "../instancias/tsp/fl3795.pcd -r 1000",
    "../instancias/tsp/nrw1379.pcd -r 1000",
    "../instancias/tsp/pcb1173.pcd -r 1000",
    "../instancias/tsp/pcb3038.pcd -r 1000",
    "../instancias/tsp/pr1002.pcd -r 1000",
    "../instancias/tsp/pr2392.pcd -r 1000",
    "../instancias/tsp/rl1304.pcd -r 1000",
    "../instancias/tsp/rl1323.pcd -r 1000",
    "../instancias/tsp/rl1889.pcd -r 1000",
    "../instancias/tsp/rl5915.pcd -r 1000",
    "../instancias/tsp/u1060.pcd -r 1000",
    "../instancias/tsp/u1432.pcd -r 1000",
    "../instancias/tsp/u2319.pcd -r 1000",
    "../instancias/tsp/vm1084.pcd -r 1000",
    "../instancias/tsp/vm1748.pcd -r 1000",
    "../instancias/tsp/pr1002.pcd -r  2000",
    "../instancias/tsp/pr2392.pcd -r  2000",
    "../instancias/tsp/rl1304.pcd -r  2000",
    "../instancias/tsp/rl1323.pcd -r  2000",
    "../instancias/tsp/rl1889.pcd -r  2000",
    "../instancias/tsp/rl5915.pcd -r  2000",
    "../instancias/tsp/vm1084.pcd -r  2000",
    "../instancias/tsp/vm1748.pcd -r  2000"
    
]



instancias = perde


# Dicionário para coletar todos os tempos e opts
dados_tempos = {instancia: [] for instancia in instancias}
dados_opts = {instancia: [] for instancia in instancias}
dados_loops = {instancia: [] for instancia in instancias}
dados_best_solution_time = {instancia: [] for instancia in instancias}
dados = {instancia: [] for instancia in instancias}

dados_csv = []



# Executar o comando para cada instância 10 vezes
for i, instancia in enumerate(instancias):
    print(f"Iniciando a instancia {instancia}")
    for _ in range(30):
        comando = f"./pcdp.run  -i {instancia} -s {_} -nsols {nsols} -init {init} -h_emph {h_emph} -warm_start {warm_start} -cpl_abort {cpl_abort} -t {time_limite} -max_age {max_age}"

        #print(comando)
        print()
        try:
            resultado = subprocess.run(comando, shell=True, capture_output=True, text=True)
            saida = resultado.stdout

            # Extrair tempo e opt da saída
            tempo = int(re.search(r"Total CMSA time: (\d+)ms", saida).group(1))
            opt = int(re.search(r"opt: (\d+)", saida).group(1))
            loops = int(re.search(r"Loops: (\d+)", saida).group(1))
            best_time = int(re.search(r"Best solution found at: (\d+) ms", saida).group(1))

            #sol size: 21
            

            dados_tempos[instancia].append(tempo)
            dados_opts[instancia].append(opt)
            dados_loops[instancia].append(loops)
            dados_best_solution_time[instancia].append(best_time)  # Adicione essa linha

            dados[instancia].append({
                "Tempo": tempo, 
                "Opt": opt, 
                "Semente": _+1, 
                "Log": saida, 
                "Loops": loops,
                "Best_solution_time": best_time  # Adicione essa linha
            })
        except Exception as e:
            tempo = 0
            opt = 0
            loops = 0
            best_time = 0

            dados_tempos[instancia].append(tempo)
            dados_opts[instancia].append(opt)
            dados_loops[instancia].append(loops)
            dados_best_solution_time[instancia].append(best_time)  # Adicione essa linha
            dados[instancia].append({
                "Tempo": tempo, 
                "Opt": opt, 
                "Semente": _+1, 
                "Log": saida, 
                "Loops": loops,
                "Best_solution_time": best_time  # Adicione essa linha
            })
            print(f"Erro ao processar a instância {instancia} com a semente {_+1}: {str(e)}")
        
    # Criar DataFrame com estatísticas

    df1 = pd.DataFrame(dados[instancia])
    df1.to_csv(f'../resultados/{nome}/resultado_{instancia.split("/")[-1]}_{nome}.csv', index=False)

    dados_csv.append((f"{instancia}",df1))

    df = pd.DataFrame({
        "Instancia": [nome[14:] for nome in instancias[:i+1]],
        "solution_medio": [np.mean(dados_opts[inst]) for inst in instancias[:i+1]],
        "Melhor_solution": [np.min(dados_opts[inst]) for inst in instancias[:i+1]],
        "Pior_solution": [np.max(dados_opts[inst]) for inst in instancias[:i+1]],
        "Desvio_padrao_solution": [np.std(dados_opts[inst]) for inst in instancias[:i+1]],
        "Loops_medio": [np.mean(dados_loops[inst]) for inst in instancias[:i+1]],
        "Tempo_medio": [np.mean(dados_tempos[inst]) for inst in instancias[:i+1]],
        "Melhor_tempo": [np.min(dados_tempos[inst]) for inst in instancias[:i+1]],
        "Pior_tempo": [np.max(dados_tempos[inst]) for inst in instancias[:i+1]],
        "Best_solution_time_medio": [np.mean(dados_best_solution_time[inst]) for inst in instancias[:i+1]],
        "Best_solution_time_min": [np.min(dados_best_solution_time[inst]) for inst in instancias[:i+1]],
        "Best_solution_time_max": [np.max(dados_best_solution_time[inst]) for inst in instancias[:i+1]],
    })

    # Salvar DataFrame de estatísticas após cada iteração de instância
    df.to_csv(f'../resultados/{nome}/{nome}.csv', index=False)
    

df = pd.DataFrame({
    "Instancia": [nome[14:] for nome in instancias[:i+1]],
    "solution_medio": [np.mean(dados_opts[inst]) for inst in instancias[:i+1]],
    "Melhor_solution": [np.min(dados_opts[inst]) for inst in instancias[:i+1]],
    "Pior_solution": [np.max(dados_opts[inst]) for inst in instancias[:i+1]],
    "Desvio_padrao_solution": [np.std(dados_opts[inst]) for inst in instancias[:i+1]],
    "Loops_medio": [np.mean(dados_loops[inst]) for inst in instancias[:i+1]],
    "Tempo_medio": [np.mean(dados_tempos[inst]) for inst in instancias[:i+1]],
    "Melhor_tempo": [np.min(dados_tempos[inst]) for inst in instancias[:i+1]],
    "Pior_tempo": [np.max(dados_tempos[inst]) for inst in instancias[:i+1]],
    "Best_solution_time_medio": [np.mean(dados_best_solution_time[inst]) for inst in instancias[:i+1]],
    "Best_solution_time_min": [np.min(dados_best_solution_time[inst]) for inst in instancias[:i+1]],
    "Best_solution_time_max": [np.max(dados_best_solution_time[inst]) for inst in instancias[:i+1]],
})
dados_csv.append((f"{nome}",df))



with open(f'../resultados/{nome}/hiperparametros.txt', 'w') as arquivo:
    arquivo.write(f"time_limite: {time_limite} \n"
                  f"nsols: {nsols} \n"
                  f"cpl_abort = {cpl_abort} \n"
                  f"init = {init} \n"
                  f"warm_start = {warm_start} \n"
                  f"h_emph = {h_emph}\n")


