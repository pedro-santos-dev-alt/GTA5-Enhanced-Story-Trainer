# Resolução de problemas: consola RPH e menu do trainer

## Sintoma observado

Ao carregar `F4`, o GTA parecia bloquear; ao carregar `F4` novamente, retomava. O menu do trainer também não aparecia.

## Causa encontrada

- `RagePluginHook.ini` definia `ConsoleKey=F4`. O `F4` abria e fechava a consola do RAGE Plugin Hook, não o trainer.
- `startup.rphs` estava vazio. Assim, o plugin `GTA5EnhancedStoryTrainer.RPH.dll` não era carregado automaticamente.
- `GTA5EnhancedStoryTrainerIndependent.addon64` é apenas uma interface de diagnóstico do ReShade. O próprio log confirma que o overlay abriu, mas esta variante não executa ações no jogo.

## Configuração corrigida nesta instalação

1. `startup.rphs` contém `LoadPlugin "GTA5EnhancedStoryTrainer.RPH.dll"`.
2. `RagePluginHook.ini` usa `ConsoleKey=F9`, para evitar abrir a consola com `F4`.
3. O plugin deve estar em `C#Plugins/GTA5EnhancedStoryTrainer.RPH.dll`.
4. Inicia o jogo através do `RAGEPluginHook.exe` e entra no modo História. O menu deve abrir quando o plugin carregar; usa `F10` para fechar/abrir e as setas, `Enter` e `Backspace` para navegar.

## Como confirmar o carregamento

- `RagePluginHook.log` deve mostrar o carregamento de `GTA5EnhancedStoryTrainer.RPH.dll` sem erro.
- `GTA5EnhancedStoryTrainer-RPH.log` deve começar com `RPH plugin loaded`.
- `GTA5EnhancedStoryTrainerIndependent.log` confirma apenas que o add-on ReShade inicializou/abriu a interface; não confirma que o plugin funcional RPH carregou.

Se o menu RPH não aparecer, fecha o jogo e envia estes dois logs: `RagePluginHook.log` e `GTA5EnhancedStoryTrainer-RPH.log`.
