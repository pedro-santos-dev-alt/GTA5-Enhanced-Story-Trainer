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
| `F7` | Abrir/fechar o menu |
| `Setas cima/baixo` | Percorrer opções |
| `Setas esquerda/direita` | Alterar veículo, arma, meteorologia, hora, slot de mod ou valores RGB |
| `Enter` | Executar/aplicar a opção selecionada |
| `Backspace` | Voltar à página principal ou fechar |

O menu usa `F7`; não usa `F6`, que fica livre para o atalho do RenoDX DLSS 5.

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
4. Inicia o jogo diretamente no modo História e carrega em `F7`.
5. Confirma no `asiloader.log`, na pasta do jogo, que aparece uma linha a carregar `GTA5EnhancedStoryTrainer.asi`.

Para remover, fecha o jogo e apaga `GTA5EnhancedStoryTrainer.asi`.

## Logs e resolução de problemas

Depois de o script arrancar, o trainer cria `GTA5EnhancedStoryTrainer.log` ao lado do `.asi`, na pasta do jogo. O ficheiro regista o arranque do script, mudanças entre modo História e sessão online, deteção de cada pressão em `F7`, mensagens de estado/erro e um heartbeat a cada 30 segundos. Não regista cada frame.

- **Não existe `GTA5EnhancedStoryTrainer.asi` no `asiloader.log`:** o ASI loader não encontrou o trainer. Confirma que o `.asi` foi extraído e copiado para a mesma pasta de `GTA5_Enhanced.exe`.
- **O `asiloader.log` lista o `.asi`, mas não existe `GTA5EnhancedStoryTrainer.log`:** o loader encontrou o ficheiro, mas o script não chegou a arrancar ou não conseguiu criar o log. Confirma a compatibilidade do ScriptHookV e as mensagens do `asiloader.log`.
- **O log mostra `F7 detected: menu opened`, mas o menu não aparece:** o input chegou ao trainer; consulta as mensagens seguintes e testa sem outros overlays/mods gráficos para isolar um conflito de renderização.
- **O log não mostra a linha de F7:** confirma que a janela do jogo tem foco e volta a testar com a tecla `F7` num teclado físico.

Para pedir ajuda, fecha o jogo e partilha `GTA5EnhancedStoryTrainer.log` e `asiloader.log`. Remove nomes de utilizador ou outros dados pessoais dos caminhos antes de os publicar.

## Safety and limitations

- Apenas modo História/offline. Não uses no GTA Online nem no FiveM. O plugin desativa o menu durante uma sessão online como proteção adicional.
- É uma primeira versão. O menu valida cada modelo antes de o criar, mas futuras atualizações do jogo podem alterar modelos ou o comportamento de natives.
- O projeto não altera ficheiros originais do GTA; chama natives do ScriptHookV durante a execução.
- O registo pode não ser criado se o ASI não carregar ou se o script não arrancar; nesse caso, começa por consultar `asiloader.log`.

## License

MIT. See [LICENSE](LICENSE).
