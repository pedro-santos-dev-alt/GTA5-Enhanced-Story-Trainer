# Changelog

## 0.4.1 - 2026-10-20

- Corrigida a causa encontrada no log: o ASI loader carregava o trainer, mas `ScriptHookV.dll` não estava carregado no processo.
- O trainer passa a carregar explicitamente `ScriptHookV.dll` a partir da sua pasta, se necessário, antes de resolver os exports e registar o script.
- O log distingue entre ScriptHookV já carregado, carregado pelo trainer ou impossível de carregar (inclui o código de erro do Windows).
- Adicionada uma verificação do build do jogo e do ScriptHookV; o trainer não carrega um hook incompatível e regista as duas versões detetadas.
- Diagnóstico desta instalação (20-10-2026): o jogo é `1.0.1158.16` e o ScriptHookV instalado é `3889.0.1158.13`; a página oficial consultada lista suporte Enhanced até `1.0.1158.13`, pelo que o menu aguarda uma versão compatível do hook.

## 0.4.0 - 2026-10-20

- Adiado o registo do script para uma thread de arranque, evitando depender de o `ScriptHookV.dll` estar pronto durante o carregamento do `.asi`.
- O trainer passa a esperar até 60 segundos pelo ScriptHookV e regista no log cada motivo de falha, incluindo DLL ausente e exports em falta.
- O `DllMain` deixa de rejeitar o plugin só porque o ScriptHookV ainda não está disponível naquele instante; o estado de carregamento fica visível no log.
- Preservado o log entre o arranque do plugin e o arranque do script, para não apagar os diagnósticos iniciais.

## 0.3.0 - 2026-10-20

- Alterado o atalho de abrir/fechar de F7 para F10 para evitar conflito com a troca de personagem configurada pelo utilizador.
- O menu passa a abrir automaticamente quando o script arranca. F10 fecha/reabre o menu e Backspace continua a voltar/fechar.
- Atualizados os controlos no overlay, o README, a secção de diagnóstico e o formato das mensagens do log.

## 0.2.0 - 2026-10-20

- Adicionado `GTA5EnhancedStoryTrainer.log` ao lado do plugin. Regista arranque do script, estado da sessão, abertura/fecho do menu com F7, mensagens de estado e heartbeat a cada 30 segundos.
- Alterada a leitura do teclado para detetar a transição de tecla solta para pressionada e amostrar os controlos em cada frame do jogo. Isto reduz a possibilidade de perder uma pressão breve de F7.
- Documentados no README a localização dos logs e os passos para distinguir falhas de instalação, arranque e deteção da tecla.
