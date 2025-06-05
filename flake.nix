{
  description = "";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = {
    self,
    nixpkgs,
  }: let
    supportedSystems = ["x86_64-linux"];
    forAllSystems = function:
      nixpkgs.lib.genAttrs supportedSystems (
        system: function nixpkgs.legacyPackages.${system}
      );
  in {
    formatter = forAllSystems (pkgs: pkgs.alejandra);

    devShells = forAllSystems (
      pkgs: let
        python-deps = ps:
          with ps; [
            pip
            flask
          ];
      in {
        default =
          pkgs.mkShell
          {
            packages = with pkgs; [
              (python3.withPackages python-deps)
              black
              uthash
              clang-tools
              cjson
            ];
          };
      }
    );
  };
}
