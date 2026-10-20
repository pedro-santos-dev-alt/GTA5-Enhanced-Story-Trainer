# Modo independente do ScriptHookV

## O que falhou na versão anterior

Os logs partilhados em 20-10-2026 mostravam que o ASI loader carregava `GTA5EnhancedStoryTrainer.asi`, mas o processo não tinha `ScriptHookV.dll` carregado. O trainer dependia dos exports do ScriptHookV para registar um script, consultar natives, ler as teclas e desenhar o menu. Sem essa inicialização, o código do menu nunca arrancava.

O ficheiro `ScriptHookV.dll` existia na pasta, mas a versão instalada era `1.0.1158.13` e o executável do jogo era `1.0.1158.16`. A versão do ScriptHookV disponível na página oficial declara suporte até à `1.0.1158.13`. Forçar o carregamento desse hook antigo não é uma correção segura: pode deixar os endereços e estruturas internas incompatíveis com o executável atual.

## O que a versão independente faz

`GTA5EnhancedStoryTrainerIndependent.addon64` é um add-on x64 do ReShade. Usa a API oficial de add-ons do ReShade e Dear ImGui para mostrar uma interface de diagnóstico. Não carrega, importa nem procura `ScriptHookV.dll`; também não usa o ASI loader.

O add-on lê o recurso de versão do executável em execução, apresenta essa versão e escreve `GTA5EnhancedStoryTrainerIndependent.log`. As páginas Jogador, Armas, Veículos e Mundo listam a funcionalidade planeada, mas marcam cada ação como indisponível.

## Limites atuais

O ReShade fornece renderização e widgets. Não fornece a interface de natives do GTA que o ScriptHookV disponibiliza. Assim, o add-on atual consegue provar e diagnosticar a camada visual independente, mas não consegue alterar vida, armas, veículos ou mundo. Não apresenta botões de cheat que pareçam funcionar sem executar ações.

Para habilitar essas ações sem ScriptHookV será necessário implementar uma camada de integração própria para o motor do jogo e validar cada endereço, estrutura e chamada no modo História. Isso implica engenharia reversa por build. Até haver um adaptador validado para uma versão específica, as ações ficam desativadas. Uma atualização do jogo mantém o overlay e a deteção de versão, mas o adaptador deverá recusar builds não reconhecidos.

## Compatibilidade e atualizações

| Componente | Compatibilidade atual |
| --- | --- |
| Interface ReShade | ReShade 6.8.0 com suporte completo a add-ons, x64 |
| GTA V Enhanced 1.0.1158.16 | Versão detetada; overlay independente do build; ações do jogo ainda não suportadas |
| ScriptHookV | Não utilizado pelo add-on independente |

O build do jogo é obtido do recurso de versão do executável a cada arranque. Para acrescentar suporte de jogo no futuro:

1. Registar o novo build do executável e obter assinaturas/offsets a partir do executável exato.
2. Implementar um adaptador isolado para esse build, sem reutilizar offsets não validados de outra versão.
3. Manter ações desligadas por padrão e exigir todas as assinaturas necessárias antes de ativar o adaptador.
4. Compilar e validar no modo História, confirmar os logs e só então atualizar a matriz de compatibilidade.

O repositório ainda não contém esses adaptadores. A tabela deverá ser atualizada apenas depois de validação real; não declarar compatibilidade só porque o projeto compila.

## Instalação

1. ReShade 6.8.0 ou compatível, instalado com **full add-on support**.
2. Descarregar o artefacto `GTA5EnhancedStoryTrainer-Independent` do GitHub Actions.
3. Copiar `GTA5EnhancedStoryTrainerIndependent.addon64` para a pasta de jogo onde está o ReShade.
4. Arrancar o GTA V Enhanced no modo História, abrir o ReShade com `Home` e selecionar `GTA V Story Trainer`.
5. Confirmar o carregamento em `ReShade.log` e a versão detetada em `GTA5EnhancedStoryTrainerIndependent.log`.

## Dependências de desenvolvimento

- Visual Studio 2022 Build Tools, carga de trabalho Desktop development with C++.
- ReShade SDK `v6.8.0` e o seu submódulo Dear ImGui. O workflow fixa esta versão para obter headers consistentes com o ReShade do utilizador.
- Windows SDK x64.

O workflow GitHub Actions descarrega o SDK oficial durante o build e publica o `.addon64` como artefacto separado. Não copia nem publica os binários do ReShade.

## Backup do mod legado

A versão ScriptHookV 0.4.1 foi preservada no computador onde este trabalho foi feito como `GTA5EnhancedStoryTrainer.asi.backup-independent-0.4.1`. O código fonte legado continua na raiz do repositório e a variante `.asi` continua a ser compilada separadamente.

## Fontes e licenças

- [ReShade API reference](https://github.com/crosire/reshade/blob/v6.8.0/REFERENCE.md)
- [Exemplo oficial de overlay do ReShade](https://github.com/crosire/reshade/blob/v6.8.0/examples/10-texture_overlay/texture_overlay_addon.cpp)
- [ReShade v6.8.0](https://github.com/crosire/reshade/tree/v6.8.0), código sob BSD-3-Clause; o Dear ImGui usado pelo SDK é MIT. O projeto consome os headers no build, sem redistribuir o SDK.
