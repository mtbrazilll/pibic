import subprocess
import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Lista de instâncias a serem testadas
instancias = ["../instancias/i4_pon.txt", "../instancias/i2_pon.txt", "../instancias/i3_pon.txt"]

# Dicionário para coletar todos os tempos e opts
dados_tempos = {instancia: [] for instancia in instancias}
dados_opts = {instancia: [] for instancia in instancias}

# Executar o comando para cada instância 10 vezes
for instancia in instancias:
    for _ in range(100):
        comando = f"./pcdp.run -l 10 -f {instancia}"
        resultado = subprocess.run(comando, shell=True, capture_output=True, text=True)
        saida = resultado.stdout

        # Extrair tempo e opt da saída
        tempo = int(re.search(r"Total CMSA time: (\d+)ms", saida).group(1))
        opt = int(re.search(r"opt: (\d+)", saida).group(1))

        dados_tempos[instancia].append(tempo)
        dados_opts[instancia].append(opt)

# Criar DataFrames
df_tempos = pd.DataFrame(dados_tempos)
df_opts = pd.DataFrame(dados_opts)

# Salvar resultados e estatísticas
df_tempos.to_csv('resultados_experimentos_tempo.csv', index=False)
df_opts.to_csv('resultados_experimentos_opt.csv', index=False)
df_tempos.describe().to_csv('estatisticas_tempos.csv')
df_opts.describe().to_csv('estatisticas_opts.csv')
