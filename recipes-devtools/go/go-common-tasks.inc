# Go binaries are not understood by the strip tool.
INHIBIT_SYSROOT_STRIP = "1"

do_compile() {
  export GOROOT_BOOTSTRAP="${GOROOT_BOOTSTRAP}"

  setup_go_arch

  setup_cgo_gcc_wrapper

  ## TODO: consider setting GO_EXTLINK_ENABLED
  export CGO_ENABLED="${GO_CROSS_CGO_ENABLED}"
  export CC=${BUILD_CC}
  export CC_FOR_TARGET="${WORKDIR}/${TARGET_PREFIX}gcc"
  export CXX_FOR_TARGET="${WORKDIR}/${TARGET_PREFIX}g++"
  export GO_GCFLAGS="${HOST_CFLAGS}"
  export GO_LDFLAGS="${HOST_LDFLAGS}"

  set > ${WORKDIR}/go-${PV}.env
  cd ${WORKDIR}/go-${PV}/go/src && bash -x ./make.bash

  # log the resulting environment
  env "GOROOT=${WORKDIR}/go-${PV}/go" "${WORKDIR}/go-${PV}/go/bin/go" env
}

do_install() {
  install -d "${D}${bindir}" "${D}${GOROOT_FINAL}"
  tar -C "${WORKDIR}/go-${PV}/go" -cf - bin lib src pkg test |
  tar -C "${D}${GOROOT_FINAL}" -xf -
  mv "${D}${GOROOT_FINAL}/bin/"* "${D}${bindir}/"

  for t in gcc g++ ; do
    cat > ${D}${GOROOT_FINAL}/bin/${TARGET_PREFIX}${t} <<EOT
#!/bin/sh
exec ${TARGET_PREFIX}${t} ${TARGET_CC_ARCH} --sysroot=${STAGING_DIR_TARGET} "\$@"
EOT
    chmod +x ${D}${GOROOT_FINAL}/bin/${TARGET_PREFIX}${t}
  done
}

## TODO: implement do_clean() and ensure we actually do rebuild super cleanly
