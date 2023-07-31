ProjectName = "GD32F427"
set_config("downloadfile", ProjectName)

-- 设置工具链
toolchain("arm-none-eabi")
	set_kind("standalone")
	--交叉编译工具路径
	set_sdkdir("D:/GNU Tools ARM Embedded/5.4 2016q3")
toolchain_end()

-- 设置编译目标
target(ProjectName)
	-- 设置生成的文件名称
	set_filename(ProjectName ..".elf")
	-- 设置编译链
	set_toolchains("arm-none-eabi")
	-- 生成二进制文件
	set_kind("binary")
	-- 启用所有警告
	set_warnings("all")
	-- 设置优化等级none smallest(os) optimize(o1) faster(02) fastest(o3)
	set_optimize("faster")
	-- 设置编译文件的目录
	set_targetdir("./build")
	
	-- 设置源文件位置
	add_files(
		"./User/*.c",
		"./Libraries/CMSIS/GD/GD32F4xx/Source/GCC/startup_gd32f4xx.s",
		"./Libraries/CMSIS/GD/GD32F4xx/Source/*.c",
		"./Libraries/GD32F4xx_standard_peripheral/Source/*.c",
		"./User/src/*.c",
		"./App/Demo/src/*.c"
	)
	
	-- 设置头文件搜索路径
	add_includedirs(
		"./User",
		"./Libraries/GD32F4xx_standard_peripheral/Include",
		"./Libraries/CMSIS/GD/GD32F4xx/Include",
		"./Libraries/CMSIS",
		"./User/inc",
		"./App/Demo/inc"
	)
	
	-- 添加宏定义
	add_defines(
		"GD32F427"
	)
	
	-- 设置C编译参数
	add_cflags(
		"-mcpu=cortex-m4",	--指定目标处理器的 CPU 架构为 Cortex-M4
		"-mthumb",	--启用 Thumb 指令集
		"-mfloat-abi=hard -mfpu=fpv4-sp-d16",	--指定浮点运算的 ABI（Application Binary Interface）为硬件浮点，同时指定使用 fpv4-sp-d16 浮点单元。
		"-fdata-sections -ffunction-sections",	--将数据段和函数放置在独立的小节中。这样可以实现对未使用代码和数据的剔除，减少生成的可执行文件大小。
		"-g -gdwarf-2",	--将数据段和函数放置在独立的小节中。这样可以实现对未使用代码和数据的剔除，减少生成的可执行文件大小。
		"--specs=nano.specs --specs=nosys.specs",	--加载特定的 C 库规范。nano.specs 是用于限制 C 库的小型版本，nosys.specs 是用于取消对系统调用的依赖。
		{ force = true }	--强制覆盖之前的编译参数设置。
	)
	
	-- 设置汇编编译参数
	add_asflags(
		"-mcpu=cortex-m4",	--指定目标处理器的 CPU 架构为 Cortex-M4
		"-mthumb",	--启用 Thumb 指令集
		"-mfloat-abi=hard -mfpu=fpv4-sp-d16",	--指定浮点运算的 ABI（Application Binary Interface）为硬件浮点，同时指定使用 fpv4-sp-d16 浮点单元。
		"-fdata-sections -ffunction-sections",	--将数据段和函数放置在独立的小节中。这样可以实现对未使用代码和数据的剔除，减少生成的可执行文件大小。
		{ force = true }
	)
	
    -- 设置链接参数
	local MapCMD = "-Wl,-Map=" .. ProjectName .. ".map,--cref"
	add_ldflags(
		"-mcpu=cortex-m4",
		"-mthumb",
		"-mfloat-abi=hard -mfpu=fpv4-sp-d16",
		"-specs=nosys.specs", -- 使用 'nosys.specs' 而不是 'nano.specs'，以避免对某些系统调用的未定义引用
		"-Tlink.ld",
		"-Wl,--gc-sections",	--启用垃圾收集功能，用于剔除未使用的代码和数据，从而减少生成的最终可执行文件的大小。
		"-u _printf_float",		--定义一个弱符号 _printf_float，用于处理浮点数的打印输出（printf）。这样的处理可能在使用浮点数格式化输出时需要。
		"-lc -lm -lnosys",		--链接 C 标准库和 math 库（libm），同时取消对系统调用的链接依赖。
		MapCMD,
		{ force = true }
	)

target_end()

--构建完成后的回调函数
after_build(function(target)
	os.exec("arm-none-eabi-objcopy -O ihex ./build/%s.elf ./build/%s.hex", target:name(), target:name())
	os.exec("arm-none-eabi-objcopy -O binary ./build/%s.elf ./build/%s.bin", target:name(), target:name())
	print("生成已完成!")
	print("********************储存空间占用情况*****************************")
	os.exec("arm-none-eabi-size -Bd ./build/%s.elf", target:name())
	print("********************储存空间占用情况*****************************")
end)


after_install(function (target) 
	os.exec("openocd -f interface/cmsis-dap.cfg -f target/stm32f4x.cfg -c \"program %s verify reset exit\"", target:targetfile())
end)