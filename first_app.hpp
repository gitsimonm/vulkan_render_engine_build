#pragma once

#include "sve_window.hpp"
#include "sve_pipeline.hpp"
#include "sve_device.hpp"
#include "sve_swap_chain.hpp"

namespace sve {
    class FirstApp {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 800;

            void run(); 

        private:
            SveWindow sveWindow{WIDTH, HEIGHT, "Hello World!"};
            SveDevice sveDevice{sveWindow};
            SveSwapChain sveSwapChain{sveDevice, sveWindow.getExtent()};
            SvePipeline svePipeline{
                sveDevice, 
                "shaders/simple_shader.vert.spv", 
                "shaders/simple_shader.frag.spv", 
                SvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
}