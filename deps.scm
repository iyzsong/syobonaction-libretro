(use-modules
 (gnu packages base)
 (gnu packages compression)
 (gnu packages fontutils)
 (gnu packages image)
 (gnu packages sdl)
 (gnu packages xiph)
 (guix download)
 (guix git-download)
 (guix gexp)
 (guix packages))


(define open-syobon-action
  (let ((version "RC3"))
    (origin
      (method git-fetch)
      (uri (git-reference
            (url "https://github.com/akemin-dayo/OpenSyobonAction")
            (commit version)))
      (file-name (git-file-name "OpenSyobonAction" version))
      (sha256
       (base32 "19x53s83mxrgy80y5bhadyzkf0rcx0nsf50hxd1hyr2k6wdzbb3v")))))


(computed-file
 "libretro-syobonaction-deps"
 (with-imported-modules '((guix build utils))
   #~(begin
       (use-modules (ice-9 match)
                    (guix build utils))
       (setenv "PATH"
               (string-join '(#$(file-append tar "/bin")
                              #$(file-append xz "/bin")
                              #$(file-append gzip "/bin")
                              #$(file-append bzip2 "/bin"))
                            ":"))
       (for-each
        (match-lambda
          ((name pkg)
           (let ((dest (string-append #$output "/" name)))
             (mkdir-p dest)
             (if (file-is-directory? pkg)
                 (copy-recursively pkg dest)
                 (invoke "tar" "-C" dest
                         "--strip-components=1"
                         "-xvf" pkg)))))
        '(("libogg" #$(package-source libogg))
          ("libvorbis" #$(package-source libvorbis))
          ("freetype" #$(package-source freetype))
          ("sdl" #$(package-source sdl))
          ("sdl-mixer" #$(package-source sdl-mixer))
          ("sdl-ttf" #$(package-source sdl-ttf))
          ("sdl-gfx" #$(package-source sdl-gfx))
          ("OpenSyobonAction" #$open-syobon-action))))))
