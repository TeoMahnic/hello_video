# Hello Video

## Configure Example
 - Open `<solution_name>.csolution.yml` in Editor.
 - Specify a Board under `target-types`.
    - Example for Alif AppKit-E7 Gen 2
        ````yaml
        target-types:
        - type: AppKit
            board: Alif Semiconductor::AppKit-E7:Gen 2
            device: Alif Semiconductor::AE722F80F55D5LS:M55_HE
        ````
 - Specify a compatible board layer under `variables:` `Board-Layer:`.
    ```yaml
    target-types:
      - type: <target_name>
        board: <board_name>
        variables:
          - Board-Layer: <board_layer>.clayer.yml
    ```
    >Board layer path can be absolute or relative to the `<project_name>.cproject.yml`.  
    >Solution directory must be on the same drive as the selected `Board-Layer`.

### Compatible board layers
- Alif AppKit-E7 Gen 2  
    required packs:
    - https://github.com/VladimirUmek/alif_ensemble_appkit-bsp/tree/video-drv
    - https://github.com/VladimirUmek/alif_ensemble-cmsis-dfp/tree/dev

## Run example (Alif AppKit-E7 Gen 2)
- Copy .bin image to SETOOLS/build/images/ folder
- Describe application in SETOOLS/build/config/custom-cfg.json
- Use SETOOLS to build ATOC and download the image  
    ```sh
    app-gen-toc -f build\config\custom-cfg.json
    app-write-mram -p
    ```

### Known issues
- Color saturation on video output (LCD)