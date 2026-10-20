# GTA V Enhanced Story Trainer

Um trainer nativo para o **modo História/offline do GTA V Enhanced**. É um projeto independente em C++ que corre como plugin `.asi`.

## Features in this first release

- **Jogador:** invencibilidade, recuperar vida, colete a 100, limpar nível de procurado e impedir que a polícia atribua estrelas.
- **Armas:** escolher e receber armas individuais, carregar um conjunto completo, munição infinita no carregador e remover todas as armas.
- **Veículos:** spawn de uma seleção de carros, motas, aeronaves, veículos de emergência e especiais; reparar e limpar; aplicar melhoramentos; escolher uma peça por slot; personalizar cores primária e secundária em RGB; pintura aleatória; matrícula `STORY`; invencibilidade.
- **Mundo:** escolher meteorologia, remover meteorologia persistente, definir a hora e parar/retomar o relógio.
- O menu bloqueia as ações quando deteta uma sessão online.

## Controls

| Key | Action |
| --- | --- |
| `F10` | Abrir/fechar o menu |
| `Setas cima/baixo` | Percorrer opções |
| `Setas esquerda/direita` | Alterar veículo, arma, meteorologia, hora, slot de mod ou valores RGB |
| `Enter` | Executar/aplicar a opção selecionada |
| `Backspace` | Voltar à página principal ou fechar |

O menu abre automaticamente quando o script do trainer arranca. Usa `F10` para fechar ou voltar a abrir. `Backspace` volta à página principal e fecha o menu quando já estás nessa página. O atalho foi mudado de `F7` para evitar conflito com a tecla de troca de personagem configurada no jogo.

## Requirements

- GTA V Enhanced para Windows, iniciado no modo História.
- Uma versão atual do ScriptHookV compatível com a versão instalada do jogo.
- ASI loader. A pasta de jogo usada para preparar este projeto já contém ScriptHookV e o ASI loader Enhanced.
- Visual Studio 2022 Build Tools com a carga de trabalho **Desktop development with C++**, se quiseres compilar localmente.

O plugin resolve as funções exportadas pelo `ScriptHookV.dll` em tempo de execução. O SDK e os binários de terceiros não estão incluídos neste repositório.

## Build

Abre `GTA5EnhancedStoryTrainer.vcxproj` no Visual Studio e compila `Release | x64`. O resultado é `build/GTA5EnhancedStoryTrainer.asi`.

O GitHub Actions também compila a versão x64 e guarda o `.asi` como artefacto da execução. O repositório não contém ficheiros do jogo nem dependências binárias externas.

## Install

1. Fecha o jogo.
2. Compila o projeto ou descarrega o artefacto `GTA5EnhancedStoryTrainer.zip` de uma execução/release de confiança e extrai `GTA5EnhancedStoryTrainer.asi`.
3. Copia `GTA5EnhancedStoryTrainer.asi` para a pasta onde está `GTA5_Enhanced.exe`, para o ASI loader o encontrar. Ter o ficheiro apenas no ZIP, na pasta de transferências ou dentro da pasta do repositório não instala o mod.
4. Inicia o jogo diretamente no modo História. O menu deve abrir automaticamente; usa `F10` para fechar/reabrir.
5. Confirma no `asiloader.log`, na pasta do jogo, que aparece uma linha a carregar `GTA5EnhancedStoryTrainer.asi`.

Para remover, fecha o jogo e apaga `GTA5EnhancedStoryTrainer.asi`.

## Logs e resolução de problemas

O trainer cria `GTA5EnhancedStoryTrainer.log` ao lado do `.asi`, na pasta do jogo. Primeiro regista o carregamento do plugin e as tentativas de encontrar o `ScriptHookV.dll`; depois regista o arranque do script, abertura automática do menu, mudanças entre modo História e sessão online, deteção de cada pressão em `F10`, mensagens de estado/erro e um heartbeat a cada 30 segundos. Não regista cada frame. A inicialização espera até 60 segundos pelo ScriptHookV, para tolerar diferenças na ordem de carregamento dos plugins.

- **Não existe `GTA5EnhancedStoryTrainer.asi` no `asiloader.log`:** o ASI loader não encontrou o trainer. Confirma que o `.asi` foi extraído e copiado para a mesma pasta de `GTA5_Enhanced.exe`.
- **O `asiloader.log` lista o `.asi` como falhado:** confirma que o ficheiro não está bloqueado pelo Windows e que corresponde à arquitetura x64. A versão atual deixa de rejeitar o plugin quando o ScriptHookV ainda não arrancou; atualiza o `.asi` e consulta o log próprio.
- **O `.asi` carrega mas não existe `GTA5EnhancedStoryTrainer.log`:** o plugin não conseguiu iniciar o logger ou a thread de arranque. Consulta `asiloader.log`; se existir, confirma permissões de escrita na pasta do jogo.
- **O log indica que `ScriptHookV.dll` não está carregado ou que falta um export:** confirma se tens a edição atual do ScriptHookV compatível com GTA V Enhanced e se o DLL está ao lado de `GTA5_Enhanced.exe`. O log repete a tentativa até 60 segundos e regista o último erro.
- **O log mostra `Trainer script registered` mas não mostra `ScriptHookV started the trainer script`:** o registo foi aceite, mas o ScriptHookV ainda não executou o script; fecha o jogo e revê a compatibilidade/versão do hook.
- **O log mostra `Menu auto-opened` ou `F10 detected: menu opened`, mas o menu não aparece:** o script arrancou; consulta as mensagens seguintes e testa sem outros overlays/mods gráficos para isolar um conflito de renderização.
- **O log não mostra a linha de F10:** confirma que a janela do jogo tem foco e volta a testar com a tecla `F10` num teclado físico.

Para pedir ajuda, fecha o jogo e partilha `GTA5EnhancedStoryTrainer.log` e `asiloader.log`. Remove nomes de utilizador ou outros dados pessoais dos caminhos antes de os publicar.

## Safety and limitations

- Apenas modo História/offline. Não uses no GTA Online nem no FiveM. O plugin desativa o menu durante uma sessão online como proteção adicional.
- É uma primeira versão. O menu valida cada modelo antes de o criar, mas futuras atualizações do jogo podem alterar modelos ou o comportamento de natives.
- O projeto não altera ficheiros originais do GTA; chama natives do ScriptHookV durante a execução.
- O registo pode não ser criado se o ASI não carregar ou se o script não arrancar; nesse caso, começa por consultar `asiloader.log`.

## License

MIT. See [LICENSE](LICENSE).
