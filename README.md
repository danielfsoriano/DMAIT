# Data traffic interruption detection and mitigation framework for RPL-based WSNs
**Artefato SBSeg 2025 Apêndice: Artigo \#10486**

Abstract: 
Wireless Sensor Networks (WSN) monitor various aspects of the environment in which they are deployed. Sensor nodes collect and send environment data by neighboring nodes until it reaches the gateway node (root node). WSNs are susceptible to several attacks that totally or partially interrupt the data flow, such as sinkhole, blackhole, gray hole/selective forwarding. We here present an application layer framework for RPL-based WSNs that detects and mitigates the attacks that interrupt data traffic while informing the root node (and the network maintainers) of the suspect nodes by alternative routes, thus preventing interception by attackers. The attacker nodes are then relegated to leaf-nodes by the local reaction of legitimate nodes, as they are prevented from acting as routers, effectively mitigating attacks. By concentrating monitoring functions on the sink, network managers can identify compromised nodes, which can lead to their physical removal. 

The experiments show that our framework kept the package loss rate at about 2\% or lower in most of those tested. Besides managing to maintain a low control message overhead (i.e., 4.67\% in a no attack scenario and 9.74\% under attack) it was able to provide the network managers with information about the location of the attackers.


# Estrutura do readme.md

O repositório está organizado da seguinte forma:
* rpl-dmait - A implementação da PoC do framework DMAIT.
* rpl-udp_baseline - A aplicação controle dos experimentos (produz o mesmo tipo de tráfego porém sem as funcionalidades do framework DMAIT).
* LogParser - Aplicação Java que processa os logs do simulador Cooja (Simulador de rede do Contiki-NG) e extrai estatísticas.
* \#CONTIKI-NG MODIFICATION - Modificação do Contiki-NG para habilitar o funcionamento da lista negativa de nós suspeitos e o framework DMAIT.
* EXPERIMENTS - Pasta contendo todos os cenários dos experimentos, separados por aplicação (DMAIT, BASELINE) e variações (ex: "-NOACKS"). Esta pasta contém todos os arquivos de cenário do simulador Cooja (.csc), além dos logs em formato bruto (\*log.txt) e os logs processados (\*log-PROCESSADO.txt).

# Selos Considerados

Esperamos que sejam considerados os seguintes selos no processo de avaliação:
- Artefatos Disponíveis (SeloD) - Devido aos fontes estarem disponíveis como DOI e os dados de execução e experimentos em repositórios públicos.
- Artefatos Funcionais (SeloF) -  Devido a listagem detalhada de todas as dependências, com as respectivas versões, das linguagens e ambientes de compilação e execução, além das instruções de instalação/execução e o exemplo de execução mínima.
- Artefatos Sustentáveis (SeloS) - Devido aos artefatos estarem modularizados e documentados conforme suas funções e modos de operação.
- Experimentos Reprodutíveis (SeloR) - Devido a descrição detalhada dos experimentos, e a disponibilização dos próprios arquivos das simulações com todos os parâmetros e sementes (seeds) usadas em cada cenário, com a finalidade de habilitar a reprodução de todos as rodadas e todos os experimentos conduzidos.

# Informações básicas

O código fonte da PoC do framework DMAIT, a aplicação BASELINE (controle dos experimentos), e o processador de logs (LogParser) estão disponíveis como um DOI e também no Github (anonimizado abaixo com o anonymous.4open.science). O processador de logs é uma aplicação Java construida para extrair e consolidar as estatísticas dos logs das simulações. Estes códigos-fonte estão disponíveis nos links abaixo.

Github anonimizado: [https://anonymous.4open.science/r/DMAIT-58BD/](https://anonymous.4open.science/r/DMAIT-58BD/).

Todos os experimentos foram criados no simulador Cooja, da própria plataforma Contiki-NG, e desta forma foram salvos como arquivos com extensão ".CSC". Estes arquivos podem ser abertos no simulador Cooja e irão trazer a configuração de cada experimento da mesma forma que foram executados por nós durante a pesquisa. Os arquivos .CSC trazem também a semente da simulação e isso faz com que os resultados obtidos com a reprodução dos experimentos sejam exatamente os mesmos obtidos durante a pesquisa. Os arquivos de todas as simulações de todos os experimentos (considerando todas as sementes usadas em todas as rodadas de execução) estão disponíveis nos repositórios abaixo.

Pasta "EXPERIMENTS" em: [https://anonymous.4open.science/r/DMAIT-58BD/](https://anonymous.4open.science/r/DMAIT-58BD/).

Google Drive: [https://drive.google.com/drive/folders/1b-GhbZBkaD_B0xJPaNTwxcVSfd0yxCbC?usp=drive_link](https://drive.google.com/drive/folders/1b-GhbZBkaD_B0xJPaNTwxcVSfd0yxCbC?usp=drive_link)

As especificações de hardware necessárias para a compilação do código são as mesmas exigidas para o funcionamento do simulador Cooja, não sendo necessário nada além disso. Dito isso, o desenvolvimento do framework foi feito em uma computador com processador quad-core e 16 GB de memória RAM, sob o sistema operacional Ubuntu 22.04.4 LTS. 

A versão do Contiki-NG Foi utilizada a versão v4.5-209, e o processo de instalação foi feito a partir da imagem Docker com o Contiki-NG disponibilizada pelos próprios desenvolvedores do Contiki-NG. O processo de instalação será descrito de forma mais detalhada na seção \ref{instalacao}.

# Dependências

O processo de instalação do Contiki-NG foi a partir da imagem Docker disponibilizada pelo próprio Contiki-NG, e assim todas as dependências necessárias para o funcionamento do Contiki-NG já estão inclusas como, por exemplo, o simulador Cooja.

Além das dependências necessárias para o funcionamento do Contiki-NG e do simulador Cooja (já resolvidas com o uso da imagem Docker oficial), para reproduzir os experimentos do artigo são apenas necessários os códigos-fonte da PoC do framework DMAIT, da aplicação BASELINE (aplicação construída como controle dos experimentos) e do processador de logs (LogParser), que extrai e consolida as estatísticas dos logs das simulações.

A compilação do framework DMAIT e aplicação BASELINE foram feitas a partir do próprio simulador Cooja que, ao abrir as simulações exige que sejam selecionados os fontes para os diferentes tipos de nós, e os compila antes da execução da simulação. Portanto nenhuma ferramenta adicional foi necessária para a compilação das aplicações.

# Preocupações com segurança

Não foram identificados riscos para os avaliados ao usar o framework Contiki-NG ou as aplicações desenvolvidas durante a pesquisa.

# Instalação

Foi utilizada a instalação do Contiki-NG a partir de imagem Docker oficial, seguindo as instruções oficiais descritas no site do Contiki-NG, cuja URL é mostrada abaixo.

[https://docs.contiki-ng.org/en/develop/doc/getting-started/Docker.html\#setup](https://docs.contiki-ng.org/en/develop/doc/getting-started/Docker.html\#setup)

As instruções recomendam baixar a imagem com a tag "contiker/contiki-ng:latest", porém essa tag referencia a última versão da imagem, que está em constante atualização pelo time do Contiki-NG. Desta forma, para baixar a imagem docker correta é necessário substituir o comando "docker pull contiker/contiki-ng" do processo de instalação oficial pelo comando: "docker image pull contiker/contiki-ng@sha256:6cb428a1d0cf2d93c8022e1a8927950a81fd9ad477804f6d77ecbbd4554bad81".

As versões mais novas da imagem do Contiki-NG utilizam o "gradle" para iniciar o simulador Cooja, mas a versão usada durante a pesquisa utilizava o "ant". Com isso basta executar o comando "ant" dentro da pasta "/tools/cooja" para iniciar o Cooja.

Os códigos fonte das aplicações DMAIT e BASELINE foram desenvolvidos a partir do exemplos contidos no próprio Contiki-NG, e devido a isso foram criados também dentro da pasta "contiki-ng/examples/". Desta forma basta copiar os códigos fonte do DMAIT e BASELINE para a pasta "contiki-ng/examples/" para reproduzir o ambiente de pesquisa. Os cenários e simulações (arquivos .CSC)" podem ser copiados para uma subpasta "EXPERIMENTS" também dentro da pasta "contiki-ng/examples", assim ficam visíveis para o container Docker.

A versão atual da PoC do framework DMAIT exige uma alteração no Contiki-NG para permitir que os nós registrem os suspeitos em uma lista negativa, e o RPL precisa de acesso a essa lista no momento quando avalia se os nós podem assumir o papel de pai. Caso um nó esteja na lista negativa o RPL precisa desconsiderá-lo como um pai viável.

Para fazer essa alteração basta substituir o arquivo "rpl-mrhof.c" em "/contiki-ng/os/net/routing/" com o disponível na pasta "\#CONTIKI-NG MODIFICATION" no Github, ou também obtido junto com os experimentos no Google Drive.


# Teste mínimo

Com o ambiente já configurado, um teste básico consiste em executar a aplicação Cooja e abrir uma simulação (por exemplo: "/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/DMAIT/TEST1-16x2-5TREE-DMAIT-SEED1.csc"). A simulação deve pedir a seleção do código fonte de cada tipo de nó usado na compilação, que devem vir pré-selecionados por padrão. Uma vez compilados os nós e a simulação aberta no simulador deve ser exibida uma janela chamada "Simulation script editor", com os dados da simulação, incluindo o tempo do experimento e o local onde o arquivo de log será salvo (que se usado o exemplo acima será: "/home/user/contiki-ng/examples/EXPERIMENTS/TEST1-16x2-5TREE-DMAIT-SEED1.txt").

Nesta janela basta então selecionar o menu "Run" e em seguida "Save simulation and run with script", que deve executar o script do começo ao fim produzindo exatamente os mesmos resultados dos experimentos feitos durante a pesquisa.

Uma vez gerado o arquivo de log do experimento o próximo passo é processar o arquivo de log com a aplicação LogParser. Essa aplicação não possui uma interface de linha de comando, então para selecionar qual é o arquivo de log a ser processado é necessário alterar o arquivo "LogParser.java" e colocar o caminho do arquivo na variável "inputLogFile". 

Uma vez processado o arquivo será então criado um outro arquivo no mesmo diretório do log original, com o mesmo nome, porém agora acrescido do sulfixo "-PROCESSADO.txt". E tanto os arquivos de log quanto os processados devem ser idênticos aos arquivos já disponibilizados.

# Experimentos

Os experimentos são a consolidação dos resultados de todas as execuções de cada cenário, conforme descrito na seção anterior. 

## Reivindicações \#1 - Redução de perda de pacotes nos cenários 5TREE

Conforme mostrado no artigo os cenários 5TREE tiveram uma redução de perda de pacotes de 24,66\% para 0,94\% quando usado o DMAIT em comparação com o cenário BASELINE.

Para reproduzir este cenário é necessário executar todos os experimentos da pasta "16x2-5TREE", em seguida processar os logs com o LogParser e consolidar as médias dos resultados.

## Reivindicações \#2 - Redução de perda de pacotes nos cenários GRID-DENSE e GRID-SPARSE

Conforme mostrado no artigo os cenários GRID-DENSE e GRID-SPARSE tiveram uma redução de perda de pacotes para valores entre 2\% e 4\% em todos os cenários quando usado o DMAIT em comparação com o cenário BASELINE.

Para reproduzir este cenário é necessário executar todos os experimentos das pastas "16x2-GRID-DENSE" e "16x2-GRID-SPARSE", em seguida processar os logs com o LogParser e consolidar as médias dos resultados.

# Informações adicionais

Para execução e confirmação de todos os outros cenários basta executar os experimentos das pastas respectivas e comparar os resultados com os logs já gerados e processados. 

# LICENSE

Licença Creative Commons Attribution 4.0 International


