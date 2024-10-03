import subprocess
import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

nome = "Wellz_5"
time_limite = "o msm do dr"
loops_construtivo = "100"

# Cria a pasta se ela não existir
if not os.path.exists(f'../resultados/{nome}'):
    os.makedirs(f'../resultados/{nome}')

# Lista de instâncias a serem testadas
instancias = [
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

instancias = [
            "../instancias/discos_variando/d1291.udc -r 1500 -l 76.1",
            "../instancias/discos_variando/d1291.udc -r 1000 -l 76.7",
            "../instancias/discos_variando/d1291.udc -r 750 -l 101.2",
            "../instancias/discos_variando/d1291.udc -r 500 -l 171.3",
            "../instancias/discos_variando/rl1889.udc -r 4000 -l 138.3",
            "../instancias/discos_variando/rl1889.udc -r 3500 -l 109.6",
            "../instancias/discos_variando/rl1889.udc -r 3000 -l 127.0",
            "../instancias/discos_variando/rl1889.udc -r 2500 -l 147.4",
            "../instancias/discos_variando/u2319.udc -r 2000 -l 214.8",
            "../instancias/discos_variando/u2319.udc -r 1700 -l 152.1",
            "../instancias/discos_variando/u2319.udc -r 1400 -l 160.1",
            "../instancias/discos_variando/u2319.udc -r 1000 -l 158.6",
            "../instancias/discos_variando/pcb3038.udc -r 1000 -l 308.2",
            "../instancias/discos_variando/pcb3038.udc -r 700 -l 265.8",
            "../instancias/discos_variando/pcb3038.udc -r 600 -l 247.5",
            "../instancias/discos_variando/pcb3038.udc -r 500 -l 292.8"]



# Dicionário para coletar todos os tempos e opts
dados_tempos = {instancia: [] for instancia in instancias}
dados_opts = {instancia: [] for instancia in instancias}
dados_loops = {instancia: [] for instancia in instancias}
dados = {instancia: [] for instancia in instancias}

dados_csv = []



# Executar o comando para cada instância 10 vezes
for i, instancia in enumerate(instancias):
    print(f"Iniciando a instancia {instancia}")
    for _ in range(100):
        comando = f"./pcdp.run  -f {instancia} -s {_+1} -c {loops_construtivo}"
        #print(comando)
        print()
        try:
            resultado = subprocess.run(comando, shell=True, capture_output=True, text=True)
            saida = resultado.stdout

            # Extrair tempo e opt da saída
            tempo = int(re.search(r"Total CMSA time: (\d+)ms", saida).group(1))
            opt = int(re.search(r"opt: (\d+)", saida).group(1))
            loops = int(re.search(r"Loops: (\d+)", saida).group(1))

            dados_tempos[instancia].append(tempo)
            dados_opts[instancia].append(opt)
            dados_loops[instancia].append(loops)
            dados[instancia].append({"Tempo": tempo, "Opt": opt, "Semente": _+1, "Log": saida, "Loops": loops})
        except Exception as e:
            tempo = 0
            opt = 0
            loops = 0

            dados_tempos[instancia].append(tempo)
            dados_opts[instancia].append(opt)
            dados_loops[instancia].append(loops)
            dados[instancia].append({"Tempo": tempo, "Opt": opt, "Semente": _+1, "Log": saida, "Loops": loops})
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
})
dados_csv.append((f"{nome}",df))

# Crie um objeto ExcelWriter
with pd.ExcelWriter(f'../resultados/{nome}/output.xlsx', engine='openpyxl') as writer:
    for nome_arquivo, df in dados_csv:
        nome_sheet = nome_arquivo.split("/")[-1].replace(".csv", "")
        df.to_excel(writer, sheet_name=nome_sheet, index=False)

with open(f'../resultados/{nome}/hiperparametros.txt', 'w') as arquivo:
    arquivo.write(f"limite de tempo: {time_limite} \n loops_construtivo: {loops_construtivo}")