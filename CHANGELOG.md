# Changelog

## 0.3.0 - 2026-10-20

- Alterado o atalho de abrir/fechar de F7 para F10 para evitar conflito com a troca de personagem configurada pelo utilizador.
- O menu passa a abrir automaticamente quando o script arranca. F10 fecha/reabre o menu e Backspace continua a voltar/fechar.
- Atualizados os controlos no overlay, o README, a secção de diagnóstico e o formato das mensagens do log.

## 0.2.0 - 2026-10-20

- Adicionado `GTA5EnhancedStoryTrainer.log` ao lado do plugin. Regista arranque do script, estado da sessão, abertura/fecho do menu com F7, mensagens de estado e heartbeat a cada 30 segundos.
- Alterada a leitura do teclado para detetar a transição de tecla solta para pressionada e amostrar os controlos em cada frame do jogo. Isto reduz a possibilidade de perder uma pressão breve de F7.
- Documentados no README a localização dos logs e os passos para distinguir falhas de instalação, arranque e deteção da tecla.
