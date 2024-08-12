import subprocess
import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

nome = "Guloso_welz"

# Cria a pasta se ela não existir
if not os.path.exists(nome):
    os.makedirs(nome)

# Lista de instâncias a serem testadas
instancias = [
            "../instancias/i1_pon.txt",
            "../instancias/i2_pon.txt",
            "../instancias/i4_pon.txt",
            "../instancias/Real-world/mona-lisa100k.udc",
            "../instancias/Real-world/nyctaxi_2.9M.udc",
            "../instancias/Real-world/uber_4.5M.udc",
            "../instancias/Real-world/usa_115K.udc",
            "../instancias/Real-world/wildfires_1.8M.udc",
            "../instancias/Real-world/world_1.9M.udc",
            "../instancias/Real-world/hail2015_10M.udc"]

limites_tempo = {'../instancias/i1_pon.txt': 200,
                '../instancias/i2_pon.txt': 200,
                '../instancias/i4_pon.txt': 200,
                '../instancias/Real-world/mona-lisa100k.udc': 10000,
                '../instancias/Real-world/nyctaxi_2.9M.udc': 10000,
                '../instancias/Real-world/uber_4.5M.udc': 10000,
                '../instancias/Real-world/usa_115K.udc': 10000,
                '../instancias/Real-world/wildfires_1.8M.udc': 10000,
                '../instancias/Real-world/world_1.9M.udc': 10000,
                '../instancias/Real-world/hail2015_10M.udc': 10000}

# Dicionário para coletar todos os tempos e opts
dados_tempos = {instancia: [] for instancia in instancias}
dados_opts = {instancia: [] for instancia in instancias}
dados_loops = {instancia: [] for instancia in instancias}
dados = {instancia: [] for instancia in instancias}

dados_csv = []



# Executar o comando para cada instância 10 vezes
for i, instancia in enumerate(instancias):
    print(f"Iniciando a instancia {instancia}")
    for _ in range(5):
        comando = f"./pcdp.run -l {limites_tempo[instancia]} -f {instancia} -s {_+1} -c 10"
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
    df1.to_csv(f'{nome}/resultado_{instancia.split("/")[-1]}_{nome}.csv', index=False)

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
    df.to_csv(f'{nome}/{nome}.csv', index=False)
    

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
with pd.ExcelWriter("output.xlsx", engine='openpyxl') as writer:
    for nome_arquivo, df in dados_csv:
        nome_sheet = nome_arquivo.split("/")[-1].replace(".csv", "")
        df.to_excel(writer, sheet_name=nome_sheet, index=False)
