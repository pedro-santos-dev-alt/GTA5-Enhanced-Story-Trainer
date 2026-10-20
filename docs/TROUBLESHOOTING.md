# Resolução de problemas: consola RPH e menu do trainer

## Sintoma observado

Ao carregar `F4`, o GTA parecia bloquear; ao carregar `F4` novamente, retomava. O menu do trainer também não aparecia.

## Causa encontrada

- `RagePluginHook.ini` definia `ConsoleKey=F4`. O `F4` abria e fechava a consola do RAGE Plugin Hook, não o trainer.
- `startup.rphs` estava vazio na primeira análise. Foi preenchido com `LoadPlugin "GTA5EnhancedStoryTrainer.RPH.dll"`.
- No arranque seguinte, o RPH leu o comando, mas o log mostrou que procurou `Plugins/GTA5EnhancedStoryTrainer.RPH.dll` e não encontrou esse caminho. O DLL estava em `C#Plugins`, diretório que não correspondeu ao caminho usado por este comando.
- `GTA5EnhancedStoryTrainerIndependent.addon64` é apenas uma interface de diagnóstico do ReShade. O próprio log confirma que o overlay abriu, mas esta variante não executa ações no jogo.

## Configuração corrigida nesta instalação

1. `startup.rphs` contém `LoadPlugin "GTA5EnhancedStoryTrainer.RPH.dll"`.
2. `RagePluginHook.ini` usa `ConsoleKey=F9`, para evitar abrir a consola com `F4`.
3. Uma cópia de `GTA5EnhancedStoryTrainer.RPH.dll` foi colocada em `Plugins`, o caminho que apareceu no erro do RPH. A cópia anterior permanece em `C#Plugins` como reserva.
4. **Ainda não foi validado um arranque do jogo depois desta correção de caminho.** O plugin deve abrir o menu ao carregar; `F10` alterna o menu e as setas, `Enter` e `Backspace` navegam.

## Como confirmar o carregamento

- `RagePluginHook.log` deve mostrar o carregamento de `Plugins/GTA5EnhancedStoryTrainer.RPH.dll` sem erro.
- `GTA5EnhancedStoryTrainer-RPH.log` deve começar com `RPH plugin loaded`.
- `GTA5EnhancedStoryTrainerIndependent.log` confirma apenas que o add-on ReShade inicializou/abriu a interface; não confirma que o plugin funcional RPH carregou.

Se o menu RPH não aparecer, fecha o jogo e envia estes dois logs: `RagePluginHook.log` e `GTA5EnhancedStoryTrainer-RPH.log`.
