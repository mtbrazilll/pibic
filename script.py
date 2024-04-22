import subprocess
import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Lista de instâncias a serem testadas
instancias = ["../instancias/Real-world/hail2015_10M.udc",
            "../instancias/Real-world/mona-lisa100k.udc",
            "../instancias/Real-world/nyctaxi_2.9M.udc",
            "../instancias/Real-world/uber_4.5M.udc",
            "../instancias/Real-world/usa_115K.udc",
            "../instancias/Real-world/wildfires_1.8M.udc",
            "../instancias/Real-world/world_1.9M.udc",
            "../instancias/i1_pon.txt",
            "../instancias/i2_pon.txt",
            "../instancias/i4_pon.txt"]

# Dicionário para coletar todos os tempos e opts
dados_tempos = {instancia: [] for instancia in instancias}
dados_opts = {instancia: [] for instancia in instancias}
dados_opts_cplex = {instancia: [] for instancia in instancias}

# Executar o comando para cada instância 10 vezes
for instancia in instancias:
    for _ in range(10):
        comando = f"./pcdp.run -l 5 -f {instancia}"
        resultado = subprocess.run(comando, shell=True, capture_output=True, text=True)
        saida = resultado.stdout

        # Extrair tempo e opt da saída
        tempo = int(re.search(r"Total CMSA time: (\d+)ms", saida).group(1))
        opt = int(re.search(r"opt: (\d+)", saida).group(1))
        cplex = int(re.search(r"cplex_otimo: (\d+)", saida).group(1))

        dados_tempos[instancia].append(tempo)
        dados_opts[instancia].append(opt)
  

# Criar DataFrames
df_tempos = pd.DataFrame(dados_tempos)
df_opts = pd.DataFrame(dados_opts)

df = pd.DataFrame({
    "Instancia": [nome[14:] for nome in instancias],
    "Mean_opt": [np.mean(_) for _ in dados_opts.values()],
    "Mean_time": [np.mean(_) for _ in dados_tempos.values()],
})

# Salvar resultados e estatísticas
df_tempos.to_csv('resultados_experimentos_tempo.csv', index=False)
df_opts.to_csv('resultados_experimentos_opt.csv', index=False)
df.to_csv('reultado.csv', index=False)
df_tempos.describe().to_csv('estatisticas_tempos.csv')
df_opts.describe().to_csv('estatisticas_opts.csv')

