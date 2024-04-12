import subprocess
import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Lista de instâncias a serem testadas
instancias = ["../instancias/i4_pon.txt", "../instancias/i2_pon.txt", "../instancias/i3_pon.txt", "../instancias/hail2015.udc",
              "../instancias/mona-lisa100k.udc", "../instancias/nyctaxi.udc", "../instancias/uber.udc", "../instancias/usa115475.udc",
              "../instancias/world.txt"]

# Dicionário para coletar todos os tempos e opts
dados_tempos = {instancia: [] for instancia in instancias}
dados_opts = {instancia: [] for instancia in instancias}

# Executar o comando para cada instância 10 vezes
for instancia in instancias:
    for _ in range(10):
        comando = f"./pcdp.run -l 5 -f {instancia}"
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

