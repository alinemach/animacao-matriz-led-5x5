# Projeto: Animação de Matriz LED 5x5

Este projeto visa criar animações em uma matriz de LEDs 5x5 utilizando o Raspberry Pi Pico. O objetivo principal é demonstrar o controle individual de LEDs em uma matriz, com animações dinâmicas que podem ser configuradas e alteradas conforme desejado.

## Objetivo

O projeto tem como objetivo a criação de animações visuais usando uma matriz LED 5x5. O Raspberry Pi Pico é utilizado para controlar a matriz através de um firmware, que é responsável pela atualização das luzes e a criação de animações personalizadas.

## Tecnologias Utilizadas

- **Raspberry Pi Pico**: microcontrolador baseado no RP2040.
- **TinyUSB**: biblioteca utilizada para comunicação USB.
- **Matriz de LEDs 5x5**: hardware composto por 25 LEDs dispostos em uma matriz 5x5.
- **Python/C/C++**: Linguagens utilizadas para desenvolver o firmware e controlar os LEDs.
- **GPIO (General Purpose Input/Output)**: para controle da matriz de LEDs.

## Estrutura do Projeto

A estrutura básica do projeto inclui:

- **Código-fonte**: Scripts responsáveis pela manipulação dos LEDs e criação das animações.
- **Configuração de hardware**: Esquemas de ligação da matriz LED 5x5 ao Raspberry Pi Pico.
- **Funções de controle**: Funções para ligar, apagar e atualizar LEDs, além de funções específicas para animação.

## Como Funciona

1. **Configuração da Matriz de LEDs**: A matriz 5x5 é conectada ao Raspberry Pi Pico através de pinos GPIO. Cada pino controla uma linha ou coluna da matriz.
   
2. **Controle de LEDs**: O firmware desenvolvido permite acionar os LEDs individualmente, acendendo-os ou apagando-os com base nas animações programadas.

3. **Animação**: As animações são definidas por sequências de estados (acender e apagar LEDs) e são executadas de forma sequencial. A duração e o ritmo das animações podem ser controlados no código.

4. **Execução**: O firmware no Raspberry Pi Pico envia sinais aos pinos GPIO para controlar quais LEDs acendem, criando a animação na matriz.

## Simulações no Wokwi e Bitidoglab

As simulações do projeto foram realizadas utilizando a plataforma **[Wokwi](https://wokwi.com/)**, que permite emular circuitos com o Raspberry Pi Pico e a matriz de LEDs 5x5. Com a ferramenta, é possível testar o funcionamento do código e ajustar as animações sem a necessidade de hardware físico.

Além disso, o projeto também foi testado em uma **placa Bitidoglab**, que é uma plataforma de prototipagem que facilita a criação e testes de circuitos com microcontroladores, oferecendo uma interface mais acessível para experimentos práticos.

## Vídeo das Animações

Um vídeo demonstrando as animações na matriz de LEDs pode ser acessado [aqui](https://github.com/alinemach/animacao-matriz-led-5x5/blob/main/animacoes.mp4). Nele, você poderá observar o efeito visual das animações, como o movimento de pontos, linhas acesas e outros efeitos criados utilizando o controle dos LEDs.

## Como Executar

1. **Requisitos**:
   - Raspberry Pi Pico configurado com o firmware do projeto.
   - Matriz de LEDs 5x5 conectada ao Raspberry Pi Pico.
   - TinyUSB configurado e funcionando.

2. **Passos**:
   - Clone este repositório para seu computador.
   - Carregue o código no Raspberry Pi Pico.
   - Conecte a matriz de LEDs aos pinos corretos no Raspberry Pi Pico.
   - Execute o firmware para ver as animações sendo exibidas na matriz de LEDs.

## Animações

- **Animação de Ponto**: Um ponto se move de um canto ao outro da matriz, criando o efeito de "rastreamento".
- **Animação de Linhas**: Linhas horizontais ou verticais de LEDs são acesas e apagadas sequencialmente.
- **Desenhos**: Desenhos simples ou letras podem ser exibidos, com LEDs acesos nas posições corretas.
       **A**: Apaga todos os LEDS.
       **B**: Liga todos os LEDs na cor azul, no nível de intensidade de 100% da luminosidade máxima.
       **C**: Liga todos os LEDs na cor vermelha, no nível de intensidade de 80% da luminosidade máxima.
       **D**: Liga todos os LEDs na cor verde, no nível de intensidade de 50% da luminosidade máxima.
       **#**: Liga todos os LEDs na cor branca, no nível de intensidade de 20% da luminosidade máxima.
       **1**: Animação com o texto **EMBARCATECH** em vermelho.
       **2**: Animação com o texto **SUBGRUPO8** em vermelho.
       **3**: Animação do Pac-Man movendo a boca, correndo do fantasma com fundo sonoro.
       **4**: Animação com o texto **EU ♥ SISTEMAS** em azul, vermelho e verde.
       **5**: Animação com o texto **..**
 

## Licença

Este projeto está licenciado sob a [Licença MIT](LICENSE).

## Contribuições

Se desejar contribuir para o projeto, por favor, faça um fork deste repositório, crie uma nova branch e envie suas mudanças através de um pull request.

---


Desenvolvido pelo Subgrupo 8 - EMBARCATECH/CEPEDI.
