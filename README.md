# Carrinho seguidor de linha com Arduino

Este repositório contém o código-fonte para controlar um carrinho utilizando um Arduino. O carrinho é capaz de seguir uma linha, detectar curvas e faixas de pedestres, e estacionar quando chegar ao fim da pista.

## Pré-requisitos

- Arduino IDE instalado no seu computador.
- Placa Arduino compatível.

## Como usar

1. Clone este repositório em sua máquina local ou faça o download dos arquivos.
2. Abra o arquivo `controle_carrinho.ino` no Arduino IDE.
3. Conecte a placa Arduino ao seu computador.
4. Selecione a placa e a porta corretas nas configurações do Arduino IDE.
5. Carregue o código para a placa Arduino.
6. Conecte os sensores e motores de acordo com as definições dos pinos no código.
7. Execute o carrinho e observe o seu comportamento.

## Descrição do código

O código está organizado da seguinte maneira:

- O método `setup()` é responsável por inicializar a comunicação serial e configurar os pinos.
- O método `loop()` é o loop principal do programa e contém as chamadas para as funções que controlam o carrinho.
- As funções `seguirLinha()`, `controlarMotoresFrente()`, `pararMotores()` e outras são responsáveis por controlar o movimento do carrinho e manter o alinhamento com a linha.
- As funções `realizarViradaParaEsquerda()`, `realizarViradaParaDireita()`, `detectarVirada()`, `detectarFaixaDePedestre()` e `detectarFimDaPista()` lidam com a detecção e o controle das curvas, faixas de pedestres e o fim da pista.
- Outras funções auxiliares são utilizadas para identificar o tipo de curva detectada, corrigir o valor do contador, detectar a presença de uma faixa de pedestres e outras tarefas específicas.

## Contribuição

Sinta-se à vontade para contribuir para este projeto. Você pode sugerir melhorias no código, adicionar novos recursos ou corrigir problemas existentes. Para contribuir, siga estas etapas:

1. Faça um fork deste repositório.
2. Crie uma branch com a sua contribuição: `git checkout -b minha-contribuicao`.
3. Faça as alterações desejadas e commit-as: `git commit -m 'Minha contribuição'`.
4. Faça o push para o seu fork: `git push origin minha-contribuicao`.
5. Abra um pull request neste repositório.