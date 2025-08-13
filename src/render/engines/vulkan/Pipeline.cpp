#include "Pipeline.hpp"
#include "Swapchain.hpp"
#include "core/File.hpp"
#include "core/Path.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Pipeline::Pipeline(const Device &device, const Swapchain &swapchain)
    : device(device),
      swapchain(swapchain) {
    // Create shader modules
    auto vertexShaderCode = File(Path("shaders/test.vert.spv")).readBinary();
    auto fragmentShaderCode = File(Path("shaders/test.frag.spv")).readBinary();
    VkShaderModule vertexShaderModule = this->createShaderModule(vertexShaderCode);
    VkShaderModule fragmentShaderModule = this->createShaderModule(fragmentShaderCode);

    // Set up shader stages
    VkPipelineShaderStageCreateInfo vertexStageInfo{};
    vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStageInfo.module = vertexShaderModule;
    vertexStageInfo.pName = "main";
    VkPipelineShaderStageCreateInfo fragmentStageInfo{};
    fragmentStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStageInfo.module = fragmentShaderModule;
    fragmentStageInfo.pName = "main";
    std::vector<VkPipelineShaderStageCreateInfo> stageInfos = {
        vertexStageInfo,
        fragmentStageInfo,
    };

    // Set up dynamic states
    VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.dynamicStateCount = 2;
    dynamicStateInfo.pDynamicStates = dynamicStates;

    // Set up fixed states
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportInfo{};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationInfo.depthClampEnable = VK_FALSE;
    rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationInfo.lineWidth = 1.0f;
    rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationInfo.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisamplingInfo{};
    multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisamplingInfo.sampleShadingEnable = VK_FALSE;
    multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    VkPipelineColorBlendStateCreateInfo colorBlendingInfo{};
    colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendingInfo.logicOpEnable = VK_FALSE;
    colorBlendingInfo.attachmentCount = 1;
    colorBlendingInfo.pAttachments = &colorBlendAttachment;

    // Create layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    if (vkCreatePipelineLayout(this->device.getVkDevice(), &pipelineLayoutInfo, nullptr, &this->layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout.");
    }

    // Create pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = stageInfos.size();
    pipelineInfo.pStages = stageInfos.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &rasterizationInfo;
    pipelineInfo.pMultisampleState = &multisamplingInfo;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlendingInfo;
    pipelineInfo.pDynamicState = &dynamicStateInfo;
    pipelineInfo.layout = this->layout;
    pipelineInfo.renderPass = this->swapchain.getVkRenderPass();
    pipelineInfo.subpass = 0;

    if (vkCreateGraphicsPipelines(
            this->device.getVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->pipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline.");
    }

    this->destroyShaderModule(vertexShaderModule);
    this->destroyShaderModule(fragmentShaderModule);
}

Pipeline::~Pipeline() {
    vkDestroyPipeline(this->device.getVkDevice(), this->pipeline, nullptr);
    vkDestroyPipelineLayout(this->device.getVkDevice(), this->layout, nullptr);
}

VkPipeline Pipeline::getVkPipeline() const noexcept {
    return this->pipeline;
}

VkShaderModule Pipeline::createShaderModule(const std::vector<char> &code) const {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(this->device.getVkDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module.");
    }

    return shaderModule;
}

void Pipeline::destroyShaderModule(VkShaderModule shaderModule) const {
    vkDestroyShaderModule(this->device.getVkDevice(), shaderModule, nullptr);
}