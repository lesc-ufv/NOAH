# NOAH: Protocolo Híbrido e Adaptativo de Codificação de Rede para Comunicação por Luz Visível

Este artefato acompanha o artigo “NOAH: Protocolo Híbrido e Adaptativo de Codificação de Rede para Comunicação por Luz Visível” e tem como objetivo permitir a reprodução dos experimentos apresentados. O trabalho propõe um protocolo desenvolvido no simulador ns-3, capaz de reduzir a perda de pacotes em até 23% quando comparado a mecanismos tradicionais, como ARQ, além de introduzir um modelo mais realista para o módulo de comunicação por luz visível (VLC) do ns-3.

O repositório disponibiliza os códigos-fonte, scripts para execução em container e instruções necessárias para a configuração do ambiente de simulação. Através deste artefato, é possível reproduzir os resultados apresentados no artigo, bem como explorar variações dos parâmetros utilizados nos experimentos.


# Estrutura do repositório

O repositório está organizado da seguinte forma:

- `ARQ/vlc/`: módulo com implementação e testes do cenário com ARQ  
- `NOAH/vlc/`: módulo com implementação e testes do cenário com NOAH  
- `NS3/vlc/`: módulo com implementação dos cenários base do NS-3  
- `Docker/`: arquivos para configuração do ambiente com Docker  
- `Resultados/`: scripts e arquivos de análise dos resultados  
- `LICENSE`: licença do projeto  
- `README.md`: documentação principal  

# Selos Considerados

Os selos considerados para avaliação deste artefato são: Disponível (D), Funcional (F) e Reprodutível(R).

# Informações básicas

Os experimentos deste artefato são executados em um ambiente containerizado utilizando Docker, garantindo portabilidade e reprodutibilidade independentemente do sistema operacional hospedeiro.

Dessa forma, a execução pode ser realizada em sistemas Linux, Windows ou macOS, desde que o Docker esteja devidamente instalado e configurado.

## Requisitos de software

- Docker
- Git (para clonagem do repositório)

# Dependências

O container é baseado na imagem Ubuntu 16.04, garantindo compatibilidade com o simulador ns-3 versão 3.25.

Durante o processo de construção do container, são instaladas automaticamente todas as dependências necessárias, incluindo:

- Compiladores: gcc, g++
- Ferramentas de build: build-essential
- Linguagens: Python 2.7 (requerido pelo ns-3.25)
- Bibliotecas: sqlite3, libsqlite3-dev, pkg-config
- Ferramentas auxiliares: git, mercurial, wget
- Suporte gráfico: qt5

Dessa forma, nenhuma instalação manual de dependências adicionais é necessária fora do ambiente Docker. No entanto, é necessário adicionar manualmente o módulo VLC e os arquivos de simulação ao diretório do ns-3, conforme descrito na Seção de Instalação.

# Preocupações com segurança

A execução deste artefato não apresenta riscos de segurança para os avaliadores.

Os experimentos são realizados em um ambiente isolado por meio de containers Docker.

# Instalação

Esta seção descreve os passos necessários para configurar o ambiente e preparar o artefato para execução.

## 1. Clonar o repositório

```bash
git clone https://github.com/lesc-ufv/NOAH
cd NOAH
```

## 2. Construir o container Docker

```bash
cd Docker
docker compose build
docker compose up -d
```

## 3. Executar o container
```bash
docker exec -it ns3-3.25 bash
```
## 4. Adicionar o módulo VLC ao ns-3
Fora do container, copie o módulo VLC do cenário que deseja testar para o diretório src/:

Substitua `/root` pelo diretório onde o arquivo está localizado:
```bash
docker cp "/root/vlc" ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/src/
```

## 5. Adicionar os scripts de simulação

Copie os arquivos de simulação para o diretório `scratch/`

Substitua `/root/vlc` pelo diretório onde o arquivo está localizado:
```bash
docker cp "/root/vlc/vlc-example.cc" ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/scratch/
```

## 6. Compilar o ns-3 com modulo VLC

Volte ao container e execute os comandos para compilar o ns-3, o processo pode demorar cerca de 1 minuto

```bash
./waf clean
./waf configure
./waf build
```
# Teste mínimo

Após a instalação para se executar o teste mínimo é necessário apenas utilizar o comando

```bash
./waf --run vlc-example
```
No qual `vlc-example` corresponde ao nome do arquivo `.cc` localizado em `scratch/`.

## Resultado esperado (exemplo)

```text
Pacote corrompido? 0

------FIM DO PACOTE-------
ID1= 88 ID2= 0
Pacote corrompido? 0

------FIM DO PACOTE-------
ID1= 89 ID2= 0
Pacote corrompido? 1
ID1= 90 ID2= 0
Pacote corrompido? 0

...

Throughput value is 303390
Number of received packets: 100
Number of good packets: 57
Distance between TX and RX is 19 meters
goodput value is 182.403
```

# Experimentos

Esta seção detalha como reproduzir parte dos resultados apresentados nos gráficos do artigo.

Os experimentos basicamente são uma repetição de passos, visto que é necessário reconfigurar o
simulador a cada modificação, que no caso se trata da adição do ARQ, adição do NOAH e uso do 
best-effort do próprio NS3.

As reinvificações, contém basicamente o mesmo passo a passo, mudando somente o arquivo que controla
o número de retransmissões máxima que o protocolo fará

## Reivindicação 1: Comparativo taxa de pacotes aceitos para 2 retransmissões
<details>
<summary>Clique para ver</summary>

### ARQ

Fora do container, copie o módulo VLC para o diretório src/ localizado dentro do container:

Substitua `/root` pelo diretório onde o arquivo está localizado:
```bash
docker cp "/root/ARQ/vlc" ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/src/
```

Ainda fora do container copie o arquivo de simulação para o diretório `scratch/`

Substitua `/root/vlc` pelo diretório onde o arquivo está localizado:
```bash
docker cp "/root/ARQ/vlc/examples/2-Retransmissoes/vlc-example.cc" ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/scratch/
```


Entre no container e na pasta `/vlc`
```bash
cd src/vlc
```
Execute o comando para realizar 10 testes e salvar nos arquivos de resultado, sendo necessario conceder a permissão ao arquivo

```bash
chmod +x testesARQ.sh
cd /opt/ns-allinone-3.25/ns-3.25
bash src/vlc/testesARQ.sh
```
### NOAH

Fora do container, copie o módulo VLC para o diretório src/ localizado dentro do container:

Substitua `/root` pelo diretório onde o arquivo está localizado:
```bash
docker cp "/root/NOAH/vlc" ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/src/
```

Ainda fora do container copie o arquivo de simulação para o diretório `scratch/`

Substitua `/root/vlc` pelo diretório onde o arquivo está localizado:
```bash
docker cp "/root/NOAH/vlc/examples/2-Retransmissoes/vlc-example.cc" ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/scratch/
```

Entre no container e na pasta `/vlc`
```bash
cd src/vlc
```
Execute o comando para realizar 10 testes e salvar nos arquivos de resultado, sendo necessario conceder a permissão ao arquivo

```bash
chmod +x testesNOAH.sh
cd /opt/ns-allinone-3.25/ns-3.25
bash src/vlc/testesNOAH.sh
```

### NS3

Fora do container, copie o módulo VLC para o diretório src/ localizado dentro do container:

Substitua `/root` pelo diretório onde o arquivo está localizado:
```bash
docker cp "/root/NS3/vlc" ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/src/
```

Ainda fora do container copie o arquivo de simulação para o diretório `scratch/`

Substitua `/root/vlc` pelo diretório onde o arquivo está localizado:
```bash
docker cp "/root/NS3/vlc/examples/vlc-example.cc" ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/scratch/
```


Entre no container e na pasta `/vlc`
```bash
cd src/vlc
```
Execute o comando para realizar 10 testes e salvar nos arquivos de resultado, sendo necessario conceder a permissão ao arquivo

```bash
chmod +x testesNS3.sh
cd /opt/ns-allinone-3.25/ns-3.25
bash src/vlc/testesNS3.sh
```

### Resultados
Novamente fora do container, na pasta de resultados rode o script responsável para recuperar os resultados gerados

```bash
chmod +x recuperarResultados.sh
./recuperarResultados.sh
```
Após a coleta dos resultados, abra o arquivo `NOAH.ipynb` dentro de `Resultados` em um ambiente Jupyter (VS Code ou navegador) e execute as células para gerar os gráficos.
</details> 

## Reivindicação 2: Comparativo taxa de pacotes aceitos para 3 retransmissões

<details>
<summary>Clique para ver</summary>

Para a realização da reinvidicação 2, visto que só sera alterado um arquivo, basta
modificar o seguinte passo, mudando a pasta onde o exemplo esta de 2 para 3 retransmissoes:

**OBS:** O restante deve ser executado normalmente como descrito na reinvidicação 1
deste os passos anteriores aos posteriores, modificando somente a pasta de onde o exemplo
será obtido

### ARQ

Ainda fora do container copie o arquivo de simulação para o diretório `scratch/`

Substitua `/root/vlc` pelo diretório onde o arquivo está localizado:
```bash
docker cp "/root/ARQ/vlc/examples/3-Retransmissoes/vlc-example.cc" ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/scratch/
```
### NOAH

Ainda fora do container copie o arquivo de simulação para o diretório `scratch/`

Substitua `/root/vlc` pelo diretório onde o arquivo está localizado:
```bash
docker cp "/root/NOAH/vlc/examples/3-Retransmissoes/vlc-example.cc" ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/scratch/
```
</details> 

## Reivindicação 3: Comparativo taxa de pacotes aceitos para 4 retransmissões

<details>
<summary>Clique para ver</summary>

Para a realização da reinvidicação 3, deve seguir o mesmo que a reinvidicação 2, basta
modificar o seguinte passo, mudando a pasta onde o exemplo esta de 2 para 3 retransmissoes:

**OBS:** O restante deve ser executado normalmente como descrito na reinvidicação 1
deste os passos anteriores aos posteriores, modificando somente a pasta de onde o exemplo
será obtido

### ARQ

Ainda fora do container copie o arquivo de simulação para o diretório `scratch/`

Substitua `/root/vlc` pelo diretório onde o arquivo está localizado:
```bash
docker cp "/root/ARQ/vlc/examples/3-Retransmissoes/vlc-example.cc" ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/scratch/
```
### NOAH

Ainda fora do container copie o arquivo de simulação para o diretório `scratch/`

Substitua `/root/vlc` pelo diretório onde o arquivo está localizado:
```bash
docker cp "/root/NOAH/vlc/examples/3-Retransmissoes/vlc-example.cc" ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/scratch/
```
</details> 

# LICENSE

Este projeto está licenciado sob a **GNU General Public License v2.0** (GPLv2).

Veja o arquivo [LICENSE](LICENSE) para mais detalhes.


