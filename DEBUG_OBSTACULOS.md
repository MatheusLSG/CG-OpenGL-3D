# Debug: obstáculos não aparecem (ex.: WSL / hardware mais fraco)

Para ajudar a descobrir por que os obstáculos às vezes não carregam na tela em algumas máquinas, o jogo grava um log de debug.

## Como gerar o log

1. Compile o jogo normalmente.
2. **Apague** o arquivo `obstaculo_debug.log` se ele já existir (para não misturar com execuções antigas).
3. Execute o jogo a partir da **pasta raiz do projeto** (onde está o executável), por exemplo:
   ```bash
   ./trabalhocg arena/arena.svg
   ```
4. Deixe a janela abrir e, se possível, use o jogo por alguns segundos (movimente, etc.).
5. Feche o jogo.
6. O arquivo **`obstaculo_debug.log`** estará na mesma pasta de onde você rodou o comando (geralmente a raiz do projeto).

## O que devolver

Envie o arquivo **`obstaculo_debug.log`** para quem estiver analisando o problema.

Se os obstáculos **não** apareceram na sua execução, informe isso ao enviar o log — assim sabemos que aquele log é de um caso em que o bug ocorreu.
