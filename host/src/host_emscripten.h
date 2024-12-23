// this implements the host-specific API utils & callbacks for emscripten

#pragma once
#include <stdio.h>

// implement these memory-helpers for each host

// copy a host-pointer to cart whenb you already have a cart-pointer
EM_JS(void, copy_to_cart_with_pointer, (u32 cartPtr, void* hostPtr, u32 size), {
  new Uint8Array(Module.cart.memory.buffer).set(Module.HEAPU8.slice(hostPtr, hostPtr+size), cartPtr);
})

// copy a cart-pointer to host when you already have a host-pointer
EM_JS(void, copy_from_cart_with_pointer, (void* hostPtr, u32 cartPtr, u32 size), {
  Module.HEAPU8.set(new Uint8Array(Module.cart.memory.buffer).slice(cartPtr, cartPtr+size), hostPtr);
})

// allocate some memory in cart
EM_JS(u32, cart_malloc, (u32 size), {
  return Module.cart.malloc(size);
})

// free some memory in cart
EM_JS(void, cart_free, (u32 ptr), {
  Module.cart.free(ptr);
})

// get the strlen of a cart-pointer
EM_JS(u32, cart_strlen, (u32 cartPtr), {
  let len = 0;
  const mem = new Uint8Array(Module.cart.memory.buffer.slice(cartPtr, cartPtr + (1024*1024)));
  while (mem[len]) len++;
  return len;
});


// this has the actual cross-host API implementation
#include "host_api.h"

// implement these callbacks for each host

// called on web-side JS to load cart into MOdule and expose host-functions to it
EM_JS(bool, em_wasm_host_load_wasm, (unsigned char* wasmBytesPtr, uint32_t wasmBytesLen, u32 width, u32 height), {
  if (!Module.canvas) {
    Module.canvas = document.getElementById("canvas");
    document.body.appendChild(Module.canvas);
  }

  Module.canvas.width = width;
  Module.canvas.height = height;
  Module.ctx = Module.canvas.getContext("2d");
  Module.screenBuffer = Module.ctx.getImageData(0, 0, Module.canvas.width, Module.canvas.height);

  const wasmBytes = Module.HEAPU8.slice(wasmBytesPtr, wasmBytesPtr+wasmBytesLen);
  const d = new TextDecoder();
  const importObject = {
    null0: {},
    wasi_snapshot_preview1: Module.wasi1_instance
  };

  // bind any host exports that start with host_
  for (const k of Object.keys(Module)) {
    if (k.startsWith('_host_')) {
      importObject.null0[k.replace(/^_host_/, "")] = Module[k];
    }
  }

  WebAssembly.instantiate(wasmBytes, importObject).then(function({instance: { exports }}){
    Module.cart = exports;
    Module.wasi1_instance.start(Module.cart);
    if (Module.cart.load) {
      Module.cart.load();
    }
  });

  return true;
});

// called on each frame
EM_JS(void, em_wasm_host_update, (pntr_color* screenBuffer), {
  if ( Module?.cart?.update){
    Module.cart.update(BigInt(Date.now()));
  }
  Module.screenBuffer.data.set(Module.HEAPU8.slice(screenBuffer, Module.canvas.width*Module.canvas.height*4));
  Module.ctx.putImageData(Module.screenBuffer, 0, 0);
})

bool wasm_host_load_wasm(unsigned char* wasmBytesPtr, uint32_t wasmBytesLen) {
  return em_wasm_host_load_wasm(wasmBytesPtr, wasmBytesLen, SCREEN_WIDTH, SCREEN_HEIGHT);
}


void wasm_host_update() {
  em_wasm_host_update(screen->data);
}


// called when cart is unloaded
void wasm_host_unload_wasm(){}
