# NOAH: Protocolo Híbrido e Adaptativo de Codificação de Rede para Comunicação por Luz Visível

Este artefato acompanha o artigo “NOAH: Protocolo Híbrido e Adaptativo de Codificação de Rede para Comunicação por Luz Visível” e tem como objetivo permitir a reprodução dos experimentos apresentados. O trabalho propõe um protocolo desenvolvido no simulador ns-3, capaz de reduzir a perda de pacotes em até 23% quando comparado a mecanismos tradicionais, como ARQ, além de introduzir um modelo mais realista para o módulo de comunicação por luz visível (VLC) do ns-3.

O repositório disponibiliza os códigos-fonte, scripts para execução em container e instruções necessárias para a configuração do ambiente de simulação. Através deste artefato, é possível reproduzir os resultados apresentados no artigo, bem como explorar variações dos parâmetros utilizados nos experimentos.


# Estrutura do repositório

O repositório está organizado da seguinte forma:

- `docker/`: contém os arquivos necessários para a construção do ambiente de execução, incluindo o `Dockerfile` e o `docker-compose.yml`.

# Selos Considerados

Os selos considerados para avaliação deste artefato são: Disponível (D) e Funcional (F).

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
docker build -t noah .
```

## 3. Executar o container
```bash
docker run -it noah
```
## 4. Adicionar o módulo VLC ao ns-3
Dentro do container, copie o módulo VLC para o diretório src/:

Substitua `/root` pelo diretório onde o arquivo está localizado:
```bash
cp -r /root/vlc /opt/ns-allinone-3.25/ns-3.25/src/
```

## 5. Adicionar os scripts de simulação

Copie os arquivos de simulação para o diretório `scratch/`

Substitua `/root/vlc` pelo diretório onde o arquivo está localizado:
```bash
cp /root/vlc/exemplo.cc /opt/ns-allinone-3.25/ns-3.25/scratch/
```

## 6. Compilar o ns-3 com modulo VLC

```bash
cd /opt/ns-allinone-3.25/ns-3.25
./waf configure
./waf build
```
# Teste mínimo

Após a instalação para se executar o teste mínimo é necessário apenas utilizar o comando

```bash
./waf --run exemplo
```
No qual `exemplo` corresponde ao nome do arquivo `.cc` localizado em `scratch/`.

# Experimentos

Esta seção deve descrever um passo a passo para a execução e obtenção dos resultados do artigo. Permitindo que os revisores consigam alcançar as reivindicações apresentadas no artigo.
Cada reivindicações deve ser apresentada em uma subseção, com detalhes de arquivos de configurações a serem alterados, comandos a serem executados, flags a serem utilizadas, tempo esperado de execução, expectativa de recursos a serem utilizados como 1GB RAM/Disk e resultado esperado.

Caso o processo para a reprodução de todos os experimentos não seja possível em tempo viável. Os autores devem escolher as principais reivindicações apresentadas no artigo e apresentar o respectivo processo para reprodução.

## Reivindicações #X

## Reivindicações #Y

# LICENSE

Apresente a licença.
