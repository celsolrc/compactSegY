# compactSegY
## Trabalho final da Disciplina Arquitetura e Programação de GPUs 2021_2


Reformatador de formato IBM Float para permitir melhorar compactação de arquivos SEG-Y, usando CUDA e baseado na proposta da dissertação de [Gustavo Rodrigues Lima](https://www.cos.ufrj.br/uploadfile/1426077898.pdf) apresentada no mestrado da COPPE UFRJ em Março de 2015.

 

## Descrição

O projeto está funcional mas ainda não lé os dados de um arquivo Seg-Y. Por conta disso, contem um projeto chamado geraDados que cria o arquivo ibmfloatdata.bin contendo um cabecalho inicial seguido por valores numéricos no formato IBM Float.

Contem uma lib com poucas funções de conversão e leitura de arquivos Seg-Y. A idéia é que futuramente essa lib seja aumentada para incorporar uma libe que eu possuo e que faz a leitura direta do arquivo Seg-Y. 

Na raiz da solution encontra-se o projeto em CUDA que realiza a leitura do arquivo ibmfloatdata.bin e gera 3 outros arquivos (restruct1floatdata.bin, restruct2floatdata.bin, restruct3floatdata.bin) contendo os valores numéricos reestruturados nos 3 formatos propostos pela dissertação de mestrado.

As informações dos nomes dos arquivos e a quantidade de dados estão configuradas como defines dentro do programa. Foram realizados testes utilizando as configurações atuais e a estrutura 1 permitiu que o resuktado compactado ficasse 30% menor que a compactação do arquivo original.
