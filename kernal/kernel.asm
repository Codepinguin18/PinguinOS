
build/kernel.elf:     file format elf32-i386


Disassembly of section .text:

00101000 <_start>:
  101000:	bc 00 00 11 00       	mov    $0x110000,%esp
  101005:	83 e4 f0             	and    $0xfffffff0,%esp
  101008:	53                   	push   %ebx
  101009:	50                   	push   %eax
  10100a:	e8 81 02 00 00       	call   101290 <cmain>
  10100f:	fa                   	cli
  101010:	f4                   	hlt
  101011:	eb fd                	jmp    101010 <_start+0x10>

00101013 <gdt_flush>:
  101013:	8b 44 24 04          	mov    0x4(%esp),%eax
  101017:	0f 01 10             	lgdtl  (%eax)
  10101a:	ea 21 10 10 00 08 00 	ljmp   $0x8,$0x101021
  101021:	66 b8 10 00          	mov    $0x10,%ax
  101025:	8e d8                	mov    %eax,%ds
  101027:	8e c0                	mov    %eax,%es
  101029:	8e e0                	mov    %eax,%fs
  10102b:	8e e8                	mov    %eax,%gs
  10102d:	8e d0                	mov    %eax,%ss
  10102f:	c3                   	ret

00101030 <tss_flush>:
  101030:	66 b8 28 00          	mov    $0x28,%ax
  101034:	0f 00 d8             	ltr    %eax
  101037:	c3                   	ret

00101038 <idt_flush>:
  101038:	8b 44 24 04          	mov    0x4(%esp),%eax
  10103c:	0f 01 18             	lidtl  (%eax)
  10103f:	c3                   	ret

00101040 <sched_switch_context>:
  101040:	8b 44 24 04          	mov    0x4(%esp),%eax
  101044:	8b 4c 24 08          	mov    0x8(%esp),%ecx
  101048:	89 38                	mov    %edi,(%eax)
  10104a:	89 70 04             	mov    %esi,0x4(%eax)
  10104d:	89 68 08             	mov    %ebp,0x8(%eax)
  101050:	89 60 0c             	mov    %esp,0xc(%eax)
  101053:	89 58 10             	mov    %ebx,0x10(%eax)
  101056:	89 50 14             	mov    %edx,0x14(%eax)
  101059:	89 48 18             	mov    %ecx,0x18(%eax)
  10105c:	89 40 1c             	mov    %eax,0x1c(%eax)
  10105f:	8b 14 24             	mov    (%esp),%edx
  101062:	89 50 20             	mov    %edx,0x20(%eax)
  101065:	9c                   	pushf
  101066:	5a                   	pop    %edx
  101067:	89 50 24             	mov    %edx,0x24(%eax)
  10106a:	8b 39                	mov    (%ecx),%edi
  10106c:	8b 71 04             	mov    0x4(%ecx),%esi
  10106f:	8b 69 08             	mov    0x8(%ecx),%ebp
  101072:	8b 61 0c             	mov    0xc(%ecx),%esp
  101075:	8b 59 10             	mov    0x10(%ecx),%ebx
  101078:	8b 51 14             	mov    0x14(%ecx),%edx
  10107b:	ff 71 24             	push   0x24(%ecx)
  10107e:	9d                   	popf
  10107f:	8b 41 1c             	mov    0x1c(%ecx),%eax
  101082:	8b 49 18             	mov    0x18(%ecx),%ecx
  101085:	c3                   	ret

00101086 <isr0>:
  101086:	6a 00                	push   $0x0
  101088:	6a 00                	push   $0x0
  10108a:	e9 53 01 00 00       	jmp    1011e2 <isr_common_stub>

0010108f <isr1>:
  10108f:	6a 00                	push   $0x0
  101091:	6a 01                	push   $0x1
  101093:	e9 4a 01 00 00       	jmp    1011e2 <isr_common_stub>

00101098 <isr2>:
  101098:	6a 00                	push   $0x0
  10109a:	6a 02                	push   $0x2
  10109c:	e9 41 01 00 00       	jmp    1011e2 <isr_common_stub>

001010a1 <isr3>:
  1010a1:	6a 00                	push   $0x0
  1010a3:	6a 03                	push   $0x3
  1010a5:	e9 38 01 00 00       	jmp    1011e2 <isr_common_stub>

001010aa <isr4>:
  1010aa:	6a 00                	push   $0x0
  1010ac:	6a 04                	push   $0x4
  1010ae:	e9 2f 01 00 00       	jmp    1011e2 <isr_common_stub>

001010b3 <isr5>:
  1010b3:	6a 00                	push   $0x0
  1010b5:	6a 05                	push   $0x5
  1010b7:	e9 26 01 00 00       	jmp    1011e2 <isr_common_stub>

001010bc <isr6>:
  1010bc:	6a 00                	push   $0x0
  1010be:	6a 06                	push   $0x6
  1010c0:	e9 1d 01 00 00       	jmp    1011e2 <isr_common_stub>

001010c5 <isr7>:
  1010c5:	6a 00                	push   $0x0
  1010c7:	6a 07                	push   $0x7
  1010c9:	e9 14 01 00 00       	jmp    1011e2 <isr_common_stub>

001010ce <isr8>:
  1010ce:	6a 08                	push   $0x8
  1010d0:	e9 0d 01 00 00       	jmp    1011e2 <isr_common_stub>

001010d5 <isr9>:
  1010d5:	6a 00                	push   $0x0
  1010d7:	6a 09                	push   $0x9
  1010d9:	e9 04 01 00 00       	jmp    1011e2 <isr_common_stub>

001010de <isr10>:
  1010de:	6a 0a                	push   $0xa
  1010e0:	e9 fd 00 00 00       	jmp    1011e2 <isr_common_stub>

001010e5 <isr11>:
  1010e5:	6a 0b                	push   $0xb
  1010e7:	e9 f6 00 00 00       	jmp    1011e2 <isr_common_stub>

001010ec <isr12>:
  1010ec:	6a 0c                	push   $0xc
  1010ee:	e9 ef 00 00 00       	jmp    1011e2 <isr_common_stub>

001010f3 <isr13>:
  1010f3:	6a 0d                	push   $0xd
  1010f5:	e9 e8 00 00 00       	jmp    1011e2 <isr_common_stub>

001010fa <isr14>:
  1010fa:	6a 0e                	push   $0xe
  1010fc:	e9 e1 00 00 00       	jmp    1011e2 <isr_common_stub>

00101101 <isr15>:
  101101:	6a 00                	push   $0x0
  101103:	6a 0f                	push   $0xf
  101105:	e9 d8 00 00 00       	jmp    1011e2 <isr_common_stub>

0010110a <isr16>:
  10110a:	6a 00                	push   $0x0
  10110c:	6a 10                	push   $0x10
  10110e:	e9 cf 00 00 00       	jmp    1011e2 <isr_common_stub>

00101113 <isr17>:
  101113:	6a 11                	push   $0x11
  101115:	e9 c8 00 00 00       	jmp    1011e2 <isr_common_stub>

0010111a <isr18>:
  10111a:	6a 00                	push   $0x0
  10111c:	6a 12                	push   $0x12
  10111e:	e9 bf 00 00 00       	jmp    1011e2 <isr_common_stub>

00101123 <isr19>:
  101123:	6a 00                	push   $0x0
  101125:	6a 13                	push   $0x13
  101127:	e9 b6 00 00 00       	jmp    1011e2 <isr_common_stub>

0010112c <isr20>:
  10112c:	6a 00                	push   $0x0
  10112e:	6a 14                	push   $0x14
  101130:	e9 ad 00 00 00       	jmp    1011e2 <isr_common_stub>

00101135 <isr21>:
  101135:	6a 15                	push   $0x15
  101137:	e9 a6 00 00 00       	jmp    1011e2 <isr_common_stub>

0010113c <isr22>:
  10113c:	6a 00                	push   $0x0
  10113e:	6a 16                	push   $0x16
  101140:	e9 9d 00 00 00       	jmp    1011e2 <isr_common_stub>

00101145 <isr23>:
  101145:	6a 00                	push   $0x0
  101147:	6a 17                	push   $0x17
  101149:	e9 94 00 00 00       	jmp    1011e2 <isr_common_stub>

0010114e <isr24>:
  10114e:	6a 00                	push   $0x0
  101150:	6a 18                	push   $0x18
  101152:	e9 8b 00 00 00       	jmp    1011e2 <isr_common_stub>

00101157 <isr25>:
  101157:	6a 00                	push   $0x0
  101159:	6a 19                	push   $0x19
  10115b:	e9 82 00 00 00       	jmp    1011e2 <isr_common_stub>

00101160 <isr26>:
  101160:	6a 00                	push   $0x0
  101162:	6a 1a                	push   $0x1a
  101164:	eb 7c                	jmp    1011e2 <isr_common_stub>

00101166 <isr27>:
  101166:	6a 00                	push   $0x0
  101168:	6a 1b                	push   $0x1b
  10116a:	eb 76                	jmp    1011e2 <isr_common_stub>

0010116c <isr28>:
  10116c:	6a 00                	push   $0x0
  10116e:	6a 1c                	push   $0x1c
  101170:	eb 70                	jmp    1011e2 <isr_common_stub>

00101172 <isr29>:
  101172:	6a 00                	push   $0x0
  101174:	6a 1d                	push   $0x1d
  101176:	eb 6a                	jmp    1011e2 <isr_common_stub>

00101178 <isr30>:
  101178:	6a 1e                	push   $0x1e
  10117a:	eb 66                	jmp    1011e2 <isr_common_stub>

0010117c <isr31>:
  10117c:	6a 00                	push   $0x0
  10117e:	6a 1f                	push   $0x1f
  101180:	eb 60                	jmp    1011e2 <isr_common_stub>

00101182 <isr32>:
  101182:	6a 00                	push   $0x0
  101184:	6a 20                	push   $0x20
  101186:	eb 5a                	jmp    1011e2 <isr_common_stub>

00101188 <isr33>:
  101188:	6a 00                	push   $0x0
  10118a:	6a 21                	push   $0x21
  10118c:	eb 54                	jmp    1011e2 <isr_common_stub>

0010118e <isr34>:
  10118e:	6a 00                	push   $0x0
  101190:	6a 22                	push   $0x22
  101192:	eb 4e                	jmp    1011e2 <isr_common_stub>

00101194 <isr35>:
  101194:	6a 00                	push   $0x0
  101196:	6a 23                	push   $0x23
  101198:	eb 48                	jmp    1011e2 <isr_common_stub>

0010119a <isr36>:
  10119a:	6a 00                	push   $0x0
  10119c:	6a 24                	push   $0x24
  10119e:	eb 42                	jmp    1011e2 <isr_common_stub>

001011a0 <isr37>:
  1011a0:	6a 00                	push   $0x0
  1011a2:	6a 25                	push   $0x25
  1011a4:	eb 3c                	jmp    1011e2 <isr_common_stub>

001011a6 <isr38>:
  1011a6:	6a 00                	push   $0x0
  1011a8:	6a 26                	push   $0x26
  1011aa:	eb 36                	jmp    1011e2 <isr_common_stub>

001011ac <isr39>:
  1011ac:	6a 00                	push   $0x0
  1011ae:	6a 27                	push   $0x27
  1011b0:	eb 30                	jmp    1011e2 <isr_common_stub>

001011b2 <isr40>:
  1011b2:	6a 00                	push   $0x0
  1011b4:	6a 28                	push   $0x28
  1011b6:	eb 2a                	jmp    1011e2 <isr_common_stub>

001011b8 <isr41>:
  1011b8:	6a 00                	push   $0x0
  1011ba:	6a 29                	push   $0x29
  1011bc:	eb 24                	jmp    1011e2 <isr_common_stub>

001011be <isr42>:
  1011be:	6a 00                	push   $0x0
  1011c0:	6a 2a                	push   $0x2a
  1011c2:	eb 1e                	jmp    1011e2 <isr_common_stub>

001011c4 <isr43>:
  1011c4:	6a 00                	push   $0x0
  1011c6:	6a 2b                	push   $0x2b
  1011c8:	eb 18                	jmp    1011e2 <isr_common_stub>

001011ca <isr44>:
  1011ca:	6a 00                	push   $0x0
  1011cc:	6a 2c                	push   $0x2c
  1011ce:	eb 12                	jmp    1011e2 <isr_common_stub>

001011d0 <isr45>:
  1011d0:	6a 00                	push   $0x0
  1011d2:	6a 2d                	push   $0x2d
  1011d4:	eb 0c                	jmp    1011e2 <isr_common_stub>

001011d6 <isr46>:
  1011d6:	6a 00                	push   $0x0
  1011d8:	6a 2e                	push   $0x2e
  1011da:	eb 06                	jmp    1011e2 <isr_common_stub>

001011dc <isr47>:
  1011dc:	6a 00                	push   $0x0
  1011de:	6a 2f                	push   $0x2f
  1011e0:	eb 00                	jmp    1011e2 <isr_common_stub>

001011e2 <isr_common_stub>:
  1011e2:	60                   	pusha
  1011e3:	1e                   	push   %ds
  1011e4:	06                   	push   %es
  1011e5:	0f a0                	push   %fs
  1011e7:	0f a8                	push   %gs
  1011e9:	66 b8 10 00          	mov    $0x10,%ax
  1011ed:	8e d8                	mov    %eax,%ds
  1011ef:	8e c0                	mov    %eax,%es
  1011f1:	8e e0                	mov    %eax,%fs
  1011f3:	8e e8                	mov    %eax,%gs
  1011f5:	54                   	push   %esp
  1011f6:	e8 b5 16 00 00       	call   1028b0 <isr_dispatch>
  1011fb:	83 c4 04             	add    $0x4,%esp
  1011fe:	0f a9                	pop    %gs
  101200:	0f a1                	pop    %fs
  101202:	07                   	pop    %es
  101203:	1f                   	pop    %ds
  101204:	61                   	popa
  101205:	83 c4 08             	add    $0x8,%esp
  101208:	cf                   	iret
  101209:	66 90                	xchg   %ax,%ax
  10120b:	66 90                	xchg   %ax,%ax
  10120d:	66 90                	xchg   %ax,%ax
  10120f:	90                   	nop

00101210 <demo_task_b>:
  101210:	56                   	push   %esi
  101211:	31 c0                	xor    %eax,%eax
  101213:	53                   	push   %ebx
  101214:	83 ec 44             	sub    $0x44,%esp
  101217:	89 e3                	mov    %esp,%ebx
  101219:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  101220:	50                   	push   %eax
  101221:	8d 70 01             	lea    0x1(%eax),%esi
  101224:	68 00 90 10 00       	push   $0x109000
  101229:	6a 40                	push   $0x40
  10122b:	53                   	push   %ebx
  10122c:	e8 6f 0c 00 00       	call   101ea0 <snprintf>
  101231:	89 1c 24             	mov    %ebx,(%esp)
  101234:	e8 b7 37 00 00       	call   1049f0 <serial_puts>
  101239:	c7 04 24 ee 02 00 00 	movl   $0x2ee,(%esp)
  101240:	e8 6b 50 00 00       	call   1062b0 <task_sleep>
  101245:	83 c4 10             	add    $0x10,%esp
  101248:	89 f0                	mov    %esi,%eax
  10124a:	eb d4                	jmp    101220 <demo_task_b+0x10>
  10124c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00101250 <demo_task_a>:
  101250:	56                   	push   %esi
  101251:	31 c0                	xor    %eax,%eax
  101253:	53                   	push   %ebx
  101254:	83 ec 44             	sub    $0x44,%esp
  101257:	89 e3                	mov    %esp,%ebx
  101259:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  101260:	50                   	push   %eax
  101261:	8d 70 01             	lea    0x1(%eax),%esi
  101264:	68 15 90 10 00       	push   $0x109015
  101269:	6a 40                	push   $0x40
  10126b:	53                   	push   %ebx
  10126c:	e8 2f 0c 00 00       	call   101ea0 <snprintf>
  101271:	89 1c 24             	mov    %ebx,(%esp)
  101274:	e8 77 37 00 00       	call   1049f0 <serial_puts>
  101279:	c7 04 24 f4 01 00 00 	movl   $0x1f4,(%esp)
  101280:	e8 2b 50 00 00       	call   1062b0 <task_sleep>
  101285:	83 c4 10             	add    $0x10,%esp
  101288:	89 f0                	mov    %esi,%eax
  10128a:	eb d4                	jmp    101260 <demo_task_a+0x10>
  10128c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00101290 <cmain>:
  101290:	56                   	push   %esi
  101291:	53                   	push   %ebx
  101292:	83 ec 04             	sub    $0x4,%esp
  101295:	8b 5c 24 10          	mov    0x10(%esp),%ebx
  101299:	e8 12 30 00 00       	call   1042b0 <vga_init>
  10129e:	e8 9d 36 00 00       	call   104940 <serial_init>
  1012a3:	83 ec 08             	sub    $0x8,%esp
  1012a6:	6a 00                	push   $0x0
  1012a8:	6a 0b                	push   $0xb
  1012aa:	e8 11 31 00 00       	call   1043c0 <vga_set_color>
  1012af:	c7 04 24 80 96 10 00 	movl   $0x109680,(%esp)
  1012b6:	e8 55 33 00 00       	call   104610 <vga_puts>
  1012bb:	5e                   	pop    %esi
  1012bc:	58                   	pop    %eax
  1012bd:	6a 00                	push   $0x0
  1012bf:	6a 07                	push   $0x7
  1012c1:	e8 fa 30 00 00       	call   1043c0 <vga_set_color>
  1012c6:	c7 04 24 a4 97 10 00 	movl   $0x1097a4,(%esp)
  1012cd:	e8 3e 33 00 00       	call   104610 <vga_puts>
  1012d2:	58                   	pop    %eax
  1012d3:	5a                   	pop    %edx
  1012d4:	68 2a 90 10 00       	push   $0x10902a
  1012d9:	6a 01                	push   $0x1
  1012db:	e8 50 38 00 00       	call   104b30 <klog>
  1012e0:	83 c4 10             	add    $0x10,%esp
  1012e3:	81 fb 02 b0 ad 2b    	cmp    $0x2badb002,%ebx
  1012e9:	74 11                	je     1012fc <cmain+0x6c>
  1012eb:	51                   	push   %ecx
  1012ec:	68 02 b0 ad 2b       	push   $0x2badb002
  1012f1:	53                   	push   %ebx
  1012f2:	68 dc 97 10 00       	push   $0x1097dc
  1012f7:	e8 14 17 00 00       	call   102a10 <kpanic>
  1012fc:	53                   	push   %ebx
  1012fd:	ff 74 24 18          	push   0x18(%esp)
  101301:	68 0c 98 10 00       	push   $0x10980c
  101306:	6a 01                	push   $0x1
  101308:	e8 23 38 00 00       	call   104b30 <klog>
  10130d:	c7 04 24 40 90 10 00 	movl   $0x109040,(%esp)
  101314:	e8 27 33 00 00       	call   104640 <vga_printf>
  101319:	e8 e2 0c 00 00       	call   102000 <gdt_init>
  10131e:	c7 04 24 4d 90 10 00 	movl   $0x10904d,(%esp)
  101325:	e8 16 33 00 00       	call   104640 <vga_printf>
  10132a:	5e                   	pop    %esi
  10132b:	58                   	pop    %eax
  10132c:	68 52 90 10 00       	push   $0x109052
  101331:	6a 01                	push   $0x1
  101333:	e8 f8 37 00 00       	call   104b30 <klog>
  101338:	c7 04 24 5e 90 10 00 	movl   $0x10905e,(%esp)
  10133f:	e8 fc 32 00 00       	call   104640 <vga_printf>
  101344:	e8 c7 0d 00 00       	call   102110 <idt_init>
  101349:	c7 04 24 4d 90 10 00 	movl   $0x10904d,(%esp)
  101350:	e8 eb 32 00 00       	call   104640 <vga_printf>
  101355:	58                   	pop    %eax
  101356:	5a                   	pop    %edx
  101357:	68 30 98 10 00       	push   $0x109830
  10135c:	6a 01                	push   $0x1
  10135e:	e8 cd 37 00 00       	call   104b30 <klog>
  101363:	c7 04 24 71 90 10 00 	movl   $0x109071,(%esp)
  10136a:	e8 d1 32 00 00       	call   104640 <vga_printf>
  10136f:	e8 dc 15 00 00       	call   102950 <cpu_init>
  101374:	c7 04 24 4d 90 10 00 	movl   $0x10904d,(%esp)
  10137b:	e8 c0 32 00 00       	call   104640 <vga_printf>
  101380:	e8 1b 16 00 00       	call   1029a0 <cpu_dump>
  101385:	e8 06 16 00 00       	call   102990 <cpu_get_info>
  10138a:	83 c4 0c             	add    $0xc,%esp
  10138d:	8d 50 0d             	lea    0xd(%eax),%edx
  101390:	52                   	push   %edx
  101391:	50                   	push   %eax
  101392:	68 7e 90 10 00       	push   $0x10907e
  101397:	e8 a4 32 00 00       	call   104640 <vga_printf>
  10139c:	c7 04 24 97 90 10 00 	movl   $0x109097,(%esp)
  1013a3:	e8 98 32 00 00       	call   104640 <vga_printf>
  1013a8:	59                   	pop    %ecx
  1013a9:	ff 74 24 20          	push   0x20(%esp)
  1013ad:	e8 fe 17 00 00       	call   102bb0 <pmm_init>
  1013b2:	e8 09 1d 00 00       	call   1030c0 <pmm_free_page_count>
  1013b7:	5b                   	pop    %ebx
  1013b8:	5e                   	pop    %esi
  1013b9:	c1 e0 0c             	shl    $0xc,%eax
  1013bc:	c1 e8 14             	shr    $0x14,%eax
  1013bf:	50                   	push   %eax
  1013c0:	68 a4 90 10 00       	push   $0x1090a4
  1013c5:	e8 76 32 00 00       	call   104640 <vga_printf>
  1013ca:	c7 04 24 b7 90 10 00 	movl   $0x1090b7,(%esp)
  1013d1:	e8 6a 32 00 00       	call   104640 <vga_printf>
  1013d6:	e8 05 1d 00 00       	call   1030e0 <paging_init>
  1013db:	c7 04 24 4d 90 10 00 	movl   $0x10904d,(%esp)
  1013e2:	e8 59 32 00 00       	call   104640 <vga_printf>
  1013e7:	c7 04 24 c7 90 10 00 	movl   $0x1090c7,(%esp)
  1013ee:	e8 4d 32 00 00       	call   104640 <vga_printf>
  1013f3:	e8 d8 1e 00 00       	call   1032d0 <heap_init>
  1013f8:	58                   	pop    %eax
  1013f9:	5a                   	pop    %edx
  1013fa:	68 00 00 00 d0       	push   $0xd0000000
  1013ff:	68 d5 90 10 00       	push   $0x1090d5
  101404:	e8 37 32 00 00       	call   104640 <vga_printf>
  101409:	c7 04 24 80 00 00 00 	movl   $0x80,(%esp)
  101410:	e8 8b 1f 00 00       	call   1033a0 <kmalloc>
  101415:	c7 04 24 40 00 00 00 	movl   $0x40,(%esp)
  10141c:	89 c3                	mov    %eax,%ebx
  10141e:	e8 dd 20 00 00       	call   103500 <kzalloc>
  101423:	83 c4 10             	add    $0x10,%esp
  101426:	89 c6                	mov    %eax,%esi
  101428:	85 db                	test   %ebx,%ebx
  10142a:	0f 84 eb 00 00 00    	je     10151b <cmain+0x28b>
  101430:	85 c0                	test   %eax,%eax
  101432:	0f 84 e3 00 00 00    	je     10151b <cmain+0x28b>
  101438:	83 ec 0c             	sub    $0xc,%esp
  10143b:	53                   	push   %ebx
  10143c:	e8 1f 21 00 00       	call   103560 <kfree>
  101441:	89 34 24             	mov    %esi,(%esp)
  101444:	e8 17 21 00 00       	call   103560 <kfree>
  101449:	59                   	pop    %ecx
  10144a:	5b                   	pop    %ebx
  10144b:	68 ff 90 10 00       	push   $0x1090ff
  101450:	6a 01                	push   $0x1
  101452:	e8 d9 36 00 00       	call   104b30 <klog>
  101457:	c7 04 24 16 91 10 00 	movl   $0x109116,(%esp)
  10145e:	e8 dd 31 00 00       	call   104640 <vga_printf>
  101463:	e8 48 38 00 00       	call   104cb0 <pci_init>
  101468:	e8 43 3b 00 00       	call   104fb0 <pci_device_count>
  10146d:	5e                   	pop    %esi
  10146e:	5a                   	pop    %edx
  10146f:	50                   	push   %eax
  101470:	68 23 91 10 00       	push   $0x109123
  101475:	e8 c6 31 00 00       	call   104640 <vga_printf>
  10147a:	e8 41 3b 00 00       	call   104fc0 <pci_dump>
  10147f:	59                   	pop    %ecx
  101480:	ff 74 24 20          	push   0x20(%esp)
  101484:	e8 07 32 00 00       	call   104690 <fb_init>
  101489:	83 c4 10             	add    $0x10,%esp
  10148c:	84 c0                	test   %al,%al
  10148e:	0f 85 94 00 00 00    	jne    101528 <cmain+0x298>
  101494:	83 ec 0c             	sub    $0xc,%esp
  101497:	68 54 91 10 00       	push   $0x109154
  10149c:	e8 9f 31 00 00       	call   104640 <vga_printf>
  1014a1:	e8 2a 4d 00 00       	call   1061d0 <sched_init>
  1014a6:	c7 04 24 4d 90 10 00 	movl   $0x10904d,(%esp)
  1014ad:	e8 8e 31 00 00       	call   104640 <vga_printf>
  1014b2:	58                   	pop    %eax
  1014b3:	5a                   	pop    %edx
  1014b4:	68 67 91 10 00       	push   $0x109167
  1014b9:	68 50 12 10 00       	push   $0x101250
  1014be:	e8 0d 4c 00 00       	call   1060d0 <task_create>
  1014c3:	59                   	pop    %ecx
  1014c4:	5b                   	pop    %ebx
  1014c5:	68 6e 91 10 00       	push   $0x10916e
  1014ca:	68 10 12 10 00       	push   $0x101210
  1014cf:	e8 fc 4b 00 00       	call   1060d0 <task_create>
  1014d4:	5e                   	pop    %esi
  1014d5:	58                   	pop    %eax
  1014d6:	6a 00                	push   $0x0
  1014d8:	6a 0a                	push   $0xa
  1014da:	e8 e1 2e 00 00       	call   1043c0 <vga_set_color>
  1014df:	e8 3c 4e 00 00       	call   106320 <sched_uptime_ms>
  1014e4:	5a                   	pop    %edx
  1014e5:	59                   	pop    %ecx
  1014e6:	50                   	push   %eax
  1014e7:	68 5c 98 10 00       	push   $0x10985c
  1014ec:	e8 4f 31 00 00       	call   104640 <vga_printf>
  1014f1:	5b                   	pop    %ebx
  1014f2:	5e                   	pop    %esi
  1014f3:	6a 00                	push   $0x0
  1014f5:	6a 07                	push   $0x7
  1014f7:	e8 c4 2e 00 00       	call   1043c0 <vga_set_color>
  1014fc:	58                   	pop    %eax
  1014fd:	5a                   	pop    %edx
  1014fe:	68 80 98 10 00       	push   $0x109880
  101503:	6a 01                	push   $0x1
  101505:	e8 26 36 00 00       	call   104b30 <klog>
  10150a:	e8 51 4e 00 00       	call   106360 <sched_dump>
  10150f:	fb                   	sti
  101510:	83 c4 10             	add    $0x10,%esp
  101513:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  101518:	f4                   	hlt
  101519:	eb fd                	jmp    101518 <cmain+0x288>
  10151b:	83 ec 0c             	sub    $0xc,%esp
  10151e:	68 e7 90 10 00       	push   $0x1090e7
  101523:	e8 e8 14 00 00       	call   102a10 <kpanic>
  101528:	83 ec 0c             	sub    $0xc,%esp
  10152b:	8b 44 24 20          	mov    0x20(%esp),%eax
  10152f:	0f b6 40 6c          	movzbl 0x6c(%eax),%eax
  101533:	50                   	push   %eax
  101534:	8b 44 24 24          	mov    0x24(%esp),%eax
  101538:	ff 70 68             	push   0x68(%eax)
  10153b:	8b 44 24 28          	mov    0x28(%esp),%eax
  10153f:	ff 70 64             	push   0x64(%eax)
  101542:	68 38 91 10 00       	push   $0x109138
  101547:	6a 01                	push   $0x1
  101549:	e8 e2 35 00 00       	call   104b30 <klog>
  10154e:	83 c4 20             	add    $0x20,%esp
  101551:	e9 3e ff ff ff       	jmp    101494 <cmain+0x204>
  101556:	66 90                	xchg   %ax,%ax
  101558:	66 90                	xchg   %ax,%ax
  10155a:	66 90                	xchg   %ax,%ax
  10155c:	66 90                	xchg   %ax,%ax
  10155e:	66 90                	xchg   %ax,%ax

00101560 <memset>:
  101560:	57                   	push   %edi
  101561:	56                   	push   %esi
  101562:	53                   	push   %ebx
  101563:	8b 7c 24 10          	mov    0x10(%esp),%edi
  101567:	8b 74 24 18          	mov    0x18(%esp),%esi
  10156b:	0f b6 54 24 14       	movzbl 0x14(%esp),%edx
  101570:	8d 0c 37             	lea    (%edi,%esi,1),%ecx
  101573:	85 f6                	test   %esi,%esi
  101575:	74 2d                	je     1015a4 <memset+0x44>
  101577:	89 ce                	mov    %ecx,%esi
  101579:	89 f8                	mov    %edi,%eax
  10157b:	29 fe                	sub    %edi,%esi
  10157d:	83 e6 01             	and    $0x1,%esi
  101580:	74 16                	je     101598 <memset+0x38>
  101582:	0f b6 5c 24 14       	movzbl 0x14(%esp),%ebx
  101587:	8d 47 01             	lea    0x1(%edi),%eax
  10158a:	88 58 ff             	mov    %bl,-0x1(%eax)
  10158d:	39 c1                	cmp    %eax,%ecx
  10158f:	74 13                	je     1015a4 <memset+0x44>
  101591:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  101598:	88 10                	mov    %dl,(%eax)
  10159a:	83 c0 02             	add    $0x2,%eax
  10159d:	88 50 ff             	mov    %dl,-0x1(%eax)
  1015a0:	39 c1                	cmp    %eax,%ecx
  1015a2:	75 f4                	jne    101598 <memset+0x38>
  1015a4:	89 f8                	mov    %edi,%eax
  1015a6:	5b                   	pop    %ebx
  1015a7:	5e                   	pop    %esi
  1015a8:	5f                   	pop    %edi
  1015a9:	c3                   	ret
  1015aa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

001015b0 <memcpy>:
  1015b0:	57                   	push   %edi
  1015b1:	56                   	push   %esi
  1015b2:	8b 44 24 14          	mov    0x14(%esp),%eax
  1015b6:	8b 54 24 0c          	mov    0xc(%esp),%edx
  1015ba:	8b 74 24 10          	mov    0x10(%esp),%esi
  1015be:	85 c0                	test   %eax,%eax
  1015c0:	74 13                	je     1015d5 <memcpy+0x25>
  1015c2:	01 d0                	add    %edx,%eax
  1015c4:	89 d7                	mov    %edx,%edi
  1015c6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1015cd:	00 
  1015ce:	66 90                	xchg   %ax,%ax
  1015d0:	a4                   	movsb  %ds:(%esi),%es:(%edi)
  1015d1:	39 f8                	cmp    %edi,%eax
  1015d3:	75 fb                	jne    1015d0 <memcpy+0x20>
  1015d5:	89 d0                	mov    %edx,%eax
  1015d7:	5e                   	pop    %esi
  1015d8:	5f                   	pop    %edi
  1015d9:	c3                   	ret
  1015da:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

001015e0 <memmove>:
  1015e0:	57                   	push   %edi
  1015e1:	56                   	push   %esi
  1015e2:	8b 54 24 0c          	mov    0xc(%esp),%edx
  1015e6:	8b 74 24 10          	mov    0x10(%esp),%esi
  1015ea:	8b 44 24 14          	mov    0x14(%esp),%eax
  1015ee:	39 f2                	cmp    %esi,%edx
  1015f0:	73 1e                	jae    101610 <memmove+0x30>
  1015f2:	8d 0c 02             	lea    (%edx,%eax,1),%ecx
  1015f5:	89 d7                	mov    %edx,%edi
  1015f7:	85 c0                	test   %eax,%eax
  1015f9:	74 0a                	je     101605 <memmove+0x25>
  1015fb:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  101600:	a4                   	movsb  %ds:(%esi),%es:(%edi)
  101601:	39 cf                	cmp    %ecx,%edi
  101603:	75 fb                	jne    101600 <memmove+0x20>
  101605:	89 d0                	mov    %edx,%eax
  101607:	5e                   	pop    %esi
  101608:	5f                   	pop    %edi
  101609:	c3                   	ret
  10160a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  101610:	85 c0                	test   %eax,%eax
  101612:	74 f1                	je     101605 <memmove+0x25>
  101614:	83 e8 01             	sub    $0x1,%eax
  101617:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10161e:	00 
  10161f:	90                   	nop
  101620:	0f b6 0c 06          	movzbl (%esi,%eax,1),%ecx
  101624:	88 0c 02             	mov    %cl,(%edx,%eax,1)
  101627:	83 e8 01             	sub    $0x1,%eax
  10162a:	73 f4                	jae    101620 <memmove+0x40>
  10162c:	89 d0                	mov    %edx,%eax
  10162e:	5e                   	pop    %esi
  10162f:	5f                   	pop    %edi
  101630:	c3                   	ret
  101631:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  101638:	00 
  101639:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00101640 <memcmp>:
  101640:	56                   	push   %esi
  101641:	53                   	push   %ebx
  101642:	8b 74 24 14          	mov    0x14(%esp),%esi
  101646:	8b 44 24 0c          	mov    0xc(%esp),%eax
  10164a:	8b 54 24 10          	mov    0x10(%esp),%edx
  10164e:	85 f6                	test   %esi,%esi
  101650:	74 2e                	je     101680 <memcmp+0x40>
  101652:	01 c6                	add    %eax,%esi
  101654:	eb 14                	jmp    10166a <memcmp+0x2a>
  101656:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10165d:	00 
  10165e:	66 90                	xchg   %ax,%ax
  101660:	83 c0 01             	add    $0x1,%eax
  101663:	83 c2 01             	add    $0x1,%edx
  101666:	39 f0                	cmp    %esi,%eax
  101668:	74 16                	je     101680 <memcmp+0x40>
  10166a:	0f b6 08             	movzbl (%eax),%ecx
  10166d:	0f b6 1a             	movzbl (%edx),%ebx
  101670:	38 d9                	cmp    %bl,%cl
  101672:	74 ec                	je     101660 <memcmp+0x20>
  101674:	0f b6 c1             	movzbl %cl,%eax
  101677:	29 d8                	sub    %ebx,%eax
  101679:	5b                   	pop    %ebx
  10167a:	5e                   	pop    %esi
  10167b:	c3                   	ret
  10167c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  101680:	31 c0                	xor    %eax,%eax
  101682:	5b                   	pop    %ebx
  101683:	5e                   	pop    %esi
  101684:	c3                   	ret
  101685:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10168c:	00 
  10168d:	8d 76 00             	lea    0x0(%esi),%esi

00101690 <strlen>:
  101690:	8b 54 24 04          	mov    0x4(%esp),%edx
  101694:	31 c0                	xor    %eax,%eax
  101696:	80 3a 00             	cmpb   $0x0,(%edx)
  101699:	74 15                	je     1016b0 <strlen+0x20>
  10169b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  1016a0:	83 c0 01             	add    $0x1,%eax
  1016a3:	80 3c 02 00          	cmpb   $0x0,(%edx,%eax,1)
  1016a7:	75 f7                	jne    1016a0 <strlen+0x10>
  1016a9:	c3                   	ret
  1016aa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1016b0:	c3                   	ret
  1016b1:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1016b8:	00 
  1016b9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

001016c0 <strcpy>:
  1016c0:	53                   	push   %ebx
  1016c1:	8b 4c 24 08          	mov    0x8(%esp),%ecx
  1016c5:	31 c0                	xor    %eax,%eax
  1016c7:	8b 5c 24 0c          	mov    0xc(%esp),%ebx
  1016cb:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  1016d0:	0f b6 14 03          	movzbl (%ebx,%eax,1),%edx
  1016d4:	88 14 01             	mov    %dl,(%ecx,%eax,1)
  1016d7:	83 c0 01             	add    $0x1,%eax
  1016da:	84 d2                	test   %dl,%dl
  1016dc:	75 f2                	jne    1016d0 <strcpy+0x10>
  1016de:	89 c8                	mov    %ecx,%eax
  1016e0:	5b                   	pop    %ebx
  1016e1:	c3                   	ret
  1016e2:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1016e9:	00 
  1016ea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

001016f0 <strncpy>:
  1016f0:	56                   	push   %esi
  1016f1:	53                   	push   %ebx
  1016f2:	8b 74 24 0c          	mov    0xc(%esp),%esi
  1016f6:	8b 4c 24 14          	mov    0x14(%esp),%ecx
  1016fa:	8b 54 24 10          	mov    0x10(%esp),%edx
  1016fe:	89 f0                	mov    %esi,%eax
  101700:	85 c9                	test   %ecx,%ecx
  101702:	75 11                	jne    101715 <strncpy+0x25>
  101704:	eb 22                	jmp    101728 <strncpy+0x38>
  101706:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10170d:	00 
  10170e:	66 90                	xchg   %ax,%ax
  101710:	83 e9 01             	sub    $0x1,%ecx
  101713:	74 13                	je     101728 <strncpy+0x38>
  101715:	0f b6 1a             	movzbl (%edx),%ebx
  101718:	83 c2 01             	add    $0x1,%edx
  10171b:	83 c0 01             	add    $0x1,%eax
  10171e:	88 58 ff             	mov    %bl,-0x1(%eax)
  101721:	84 db                	test   %bl,%bl
  101723:	75 eb                	jne    101710 <strncpy+0x20>
  101725:	c6 00 00             	movb   $0x0,(%eax)
  101728:	89 f0                	mov    %esi,%eax
  10172a:	5b                   	pop    %ebx
  10172b:	5e                   	pop    %esi
  10172c:	c3                   	ret
  10172d:	8d 76 00             	lea    0x0(%esi),%esi

00101730 <strcat>:
  101730:	56                   	push   %esi
  101731:	53                   	push   %ebx
  101732:	8b 74 24 0c          	mov    0xc(%esp),%esi
  101736:	8b 5c 24 10          	mov    0x10(%esp),%ebx
  10173a:	80 3e 00             	cmpb   $0x0,(%esi)
  10173d:	89 f2                	mov    %esi,%edx
  10173f:	74 0f                	je     101750 <strcat+0x20>
  101741:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  101748:	83 c2 01             	add    $0x1,%edx
  10174b:	80 3a 00             	cmpb   $0x0,(%edx)
  10174e:	75 f8                	jne    101748 <strcat+0x18>
  101750:	31 c0                	xor    %eax,%eax
  101752:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  101758:	0f b6 0c 03          	movzbl (%ebx,%eax,1),%ecx
  10175c:	88 0c 02             	mov    %cl,(%edx,%eax,1)
  10175f:	83 c0 01             	add    $0x1,%eax
  101762:	84 c9                	test   %cl,%cl
  101764:	75 f2                	jne    101758 <strcat+0x28>
  101766:	89 f0                	mov    %esi,%eax
  101768:	5b                   	pop    %ebx
  101769:	5e                   	pop    %esi
  10176a:	c3                   	ret
  10176b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00101770 <strcmp>:
  101770:	53                   	push   %ebx
  101771:	8b 54 24 08          	mov    0x8(%esp),%edx
  101775:	8b 4c 24 0c          	mov    0xc(%esp),%ecx
  101779:	0f b6 02             	movzbl (%edx),%eax
  10177c:	84 c0                	test   %al,%al
  10177e:	75 18                	jne    101798 <strcmp+0x28>
  101780:	eb 30                	jmp    1017b2 <strcmp+0x42>
  101782:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  101788:	0f b6 42 01          	movzbl 0x1(%edx),%eax
  10178c:	83 c2 01             	add    $0x1,%edx
  10178f:	8d 59 01             	lea    0x1(%ecx),%ebx
  101792:	84 c0                	test   %al,%al
  101794:	74 12                	je     1017a8 <strcmp+0x38>
  101796:	89 d9                	mov    %ebx,%ecx
  101798:	0f b6 19             	movzbl (%ecx),%ebx
  10179b:	38 c3                	cmp    %al,%bl
  10179d:	74 e9                	je     101788 <strcmp+0x18>
  10179f:	29 d8                	sub    %ebx,%eax
  1017a1:	5b                   	pop    %ebx
  1017a2:	c3                   	ret
  1017a3:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  1017a8:	0f b6 59 01          	movzbl 0x1(%ecx),%ebx
  1017ac:	31 c0                	xor    %eax,%eax
  1017ae:	29 d8                	sub    %ebx,%eax
  1017b0:	5b                   	pop    %ebx
  1017b1:	c3                   	ret
  1017b2:	0f b6 19             	movzbl (%ecx),%ebx
  1017b5:	31 c0                	xor    %eax,%eax
  1017b7:	eb e6                	jmp    10179f <strcmp+0x2f>
  1017b9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

001017c0 <strncmp>:
  1017c0:	53                   	push   %ebx
  1017c1:	8b 54 24 10          	mov    0x10(%esp),%edx
  1017c5:	8b 44 24 08          	mov    0x8(%esp),%eax
  1017c9:	8b 4c 24 0c          	mov    0xc(%esp),%ecx
  1017cd:	85 d2                	test   %edx,%edx
  1017cf:	75 16                	jne    1017e7 <strncmp+0x27>
  1017d1:	eb 2d                	jmp    101800 <strncmp+0x40>
  1017d3:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  1017d8:	3a 19                	cmp    (%ecx),%bl
  1017da:	75 12                	jne    1017ee <strncmp+0x2e>
  1017dc:	83 c0 01             	add    $0x1,%eax
  1017df:	83 c1 01             	add    $0x1,%ecx
  1017e2:	83 ea 01             	sub    $0x1,%edx
  1017e5:	74 19                	je     101800 <strncmp+0x40>
  1017e7:	0f b6 18             	movzbl (%eax),%ebx
  1017ea:	84 db                	test   %bl,%bl
  1017ec:	75 ea                	jne    1017d8 <strncmp+0x18>
  1017ee:	0f b6 00             	movzbl (%eax),%eax
  1017f1:	0f b6 11             	movzbl (%ecx),%edx
  1017f4:	5b                   	pop    %ebx
  1017f5:	29 d0                	sub    %edx,%eax
  1017f7:	c3                   	ret
  1017f8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1017ff:	00 
  101800:	31 c0                	xor    %eax,%eax
  101802:	5b                   	pop    %ebx
  101803:	c3                   	ret
  101804:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10180b:	00 
  10180c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00101810 <strchr>:
  101810:	8b 44 24 04          	mov    0x4(%esp),%eax
  101814:	0f b6 10             	movzbl (%eax),%edx
  101817:	84 d2                	test   %dl,%dl
  101819:	74 25                	je     101840 <strchr+0x30>
  10181b:	0f b6 4c 24 08       	movzbl 0x8(%esp),%ecx
  101820:	eb 11                	jmp    101833 <strchr+0x23>
  101822:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  101828:	0f b6 50 01          	movzbl 0x1(%eax),%edx
  10182c:	83 c0 01             	add    $0x1,%eax
  10182f:	84 d2                	test   %dl,%dl
  101831:	74 0d                	je     101840 <strchr+0x30>
  101833:	38 d1                	cmp    %dl,%cl
  101835:	75 f1                	jne    101828 <strchr+0x18>
  101837:	c3                   	ret
  101838:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10183f:	00 
  101840:	31 c0                	xor    %eax,%eax
  101842:	c3                   	ret
  101843:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10184a:	00 
  10184b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00101850 <strrev>:
  101850:	57                   	push   %edi
  101851:	56                   	push   %esi
  101852:	53                   	push   %ebx
  101853:	8b 5c 24 10          	mov    0x10(%esp),%ebx
  101857:	80 3b 00             	cmpb   $0x0,(%ebx)
  10185a:	74 3a                	je     101896 <strrev+0x46>
  10185c:	31 c0                	xor    %eax,%eax
  10185e:	66 90                	xchg   %ax,%ax
  101860:	89 c2                	mov    %eax,%edx
  101862:	83 c0 01             	add    $0x1,%eax
  101865:	80 3c 03 00          	cmpb   $0x0,(%ebx,%eax,1)
  101869:	75 f5                	jne    101860 <strrev+0x10>
  10186b:	d1 e8                	shr    $1,%eax
  10186d:	74 27                	je     101896 <strrev+0x46>
  10186f:	89 de                	mov    %ebx,%esi
  101871:	01 da                	add    %ebx,%edx
  101873:	8d 3c 18             	lea    (%eax,%ebx,1),%edi
  101876:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10187d:	00 
  10187e:	66 90                	xchg   %ax,%ax
  101880:	0f b6 02             	movzbl (%edx),%eax
  101883:	0f b6 0e             	movzbl (%esi),%ecx
  101886:	83 c6 01             	add    $0x1,%esi
  101889:	83 ea 01             	sub    $0x1,%edx
  10188c:	88 46 ff             	mov    %al,-0x1(%esi)
  10188f:	88 4a 01             	mov    %cl,0x1(%edx)
  101892:	39 fe                	cmp    %edi,%esi
  101894:	75 ea                	jne    101880 <strrev+0x30>
  101896:	89 d8                	mov    %ebx,%eax
  101898:	5b                   	pop    %ebx
  101899:	5e                   	pop    %esi
  10189a:	5f                   	pop    %edi
  10189b:	c3                   	ret
  10189c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

001018a0 <uitoa>:
  1018a0:	55                   	push   %ebp
  1018a1:	ba 60 a5 10 00       	mov    $0x10a560,%edx
  1018a6:	b8 a0 a5 10 00       	mov    $0x10a5a0,%eax
  1018ab:	57                   	push   %edi
  1018ac:	56                   	push   %esi
  1018ad:	53                   	push   %ebx
  1018ae:	83 ec 08             	sub    $0x8,%esp
  1018b1:	80 7c 24 28 00       	cmpb   $0x0,0x28(%esp)
  1018b6:	8b 6c 24 24          	mov    0x24(%esp),%ebp
  1018ba:	0f 44 c2             	cmove  %edx,%eax
  1018bd:	8b 5c 24 1c          	mov    0x1c(%esp),%ebx
  1018c1:	8b 7c 24 20          	mov    0x20(%esp),%edi
  1018c5:	89 c1                	mov    %eax,%ecx
  1018c7:	8d 45 fe             	lea    -0x2(%ebp),%eax
  1018ca:	83 f8 22             	cmp    $0x22,%eax
  1018cd:	0f 87 85 00 00 00    	ja     101958 <uitoa+0xb8>
  1018d3:	85 ff                	test   %edi,%edi
  1018d5:	75 29                	jne    101900 <uitoa+0x60>
  1018d7:	c6 03 30             	movb   $0x30,(%ebx)
  1018da:	be 01 00 00 00       	mov    $0x1,%esi
  1018df:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  1018e6:	8b 04 24             	mov    (%esp),%eax
  1018e9:	c6 04 03 00          	movb   $0x0,(%ebx,%eax,1)
  1018ed:	83 c4 08             	add    $0x8,%esp
  1018f0:	89 f0                	mov    %esi,%eax
  1018f2:	5b                   	pop    %ebx
  1018f3:	5e                   	pop    %esi
  1018f4:	5f                   	pop    %edi
  1018f5:	5d                   	pop    %ebp
  1018f6:	c3                   	ret
  1018f7:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1018fe:	00 
  1018ff:	90                   	nop
  101900:	31 f6                	xor    %esi,%esi
  101902:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  101908:	89 f8                	mov    %edi,%eax
  10190a:	31 d2                	xor    %edx,%edx
  10190c:	89 74 24 04          	mov    %esi,0x4(%esp)
  101910:	83 c6 01             	add    $0x1,%esi
  101913:	f7 f5                	div    %ebp
  101915:	89 34 24             	mov    %esi,(%esp)
  101918:	0f b6 14 11          	movzbl (%ecx,%edx,1),%edx
  10191c:	88 54 33 ff          	mov    %dl,-0x1(%ebx,%esi,1)
  101920:	89 fa                	mov    %edi,%edx
  101922:	89 c7                	mov    %eax,%edi
  101924:	39 ea                	cmp    %ebp,%edx
  101926:	73 e0                	jae    101908 <uitoa+0x68>
  101928:	8b 4c 24 04          	mov    0x4(%esp),%ecx
  10192c:	85 c9                	test   %ecx,%ecx
  10192e:	74 b6                	je     1018e6 <uitoa+0x46>
  101930:	31 d2                	xor    %edx,%edx
  101932:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  101938:	0f b6 3c 13          	movzbl (%ebx,%edx,1),%edi
  10193c:	0f b6 04 0b          	movzbl (%ebx,%ecx,1),%eax
  101940:	88 04 13             	mov    %al,(%ebx,%edx,1)
  101943:	89 f8                	mov    %edi,%eax
  101945:	83 c2 01             	add    $0x1,%edx
  101948:	88 04 0b             	mov    %al,(%ebx,%ecx,1)
  10194b:	83 e9 01             	sub    $0x1,%ecx
  10194e:	39 ca                	cmp    %ecx,%edx
  101950:	7c e6                	jl     101938 <uitoa+0x98>
  101952:	eb 92                	jmp    1018e6 <uitoa+0x46>
  101954:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  101958:	c6 03 00             	movb   $0x0,(%ebx)
  10195b:	31 f6                	xor    %esi,%esi
  10195d:	83 c4 08             	add    $0x8,%esp
  101960:	5b                   	pop    %ebx
  101961:	89 f0                	mov    %esi,%eax
  101963:	5e                   	pop    %esi
  101964:	5f                   	pop    %edi
  101965:	5d                   	pop    %ebp
  101966:	c3                   	ret
  101967:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10196e:	00 
  10196f:	90                   	nop

00101970 <itoa>:
  101970:	53                   	push   %ebx
  101971:	8b 44 24 0c          	mov    0xc(%esp),%eax
  101975:	31 db                	xor    %ebx,%ebx
  101977:	8b 54 24 08          	mov    0x8(%esp),%edx
  10197b:	85 c0                	test   %eax,%eax
  10197d:	79 0d                	jns    10198c <itoa+0x1c>
  10197f:	c6 02 2d             	movb   $0x2d,(%edx)
  101982:	f7 d8                	neg    %eax
  101984:	83 c2 01             	add    $0x1,%edx
  101987:	bb 01 00 00 00       	mov    $0x1,%ebx
  10198c:	6a 00                	push   $0x0
  10198e:	6a 0a                	push   $0xa
  101990:	50                   	push   %eax
  101991:	52                   	push   %edx
  101992:	e8 09 ff ff ff       	call   1018a0 <uitoa>
  101997:	83 c4 10             	add    $0x10,%esp
  10199a:	01 d8                	add    %ebx,%eax
  10199c:	5b                   	pop    %ebx
  10199d:	c3                   	ret
  10199e:	66 90                	xchg   %ax,%ax

001019a0 <vsnprintf>:
  1019a0:	55                   	push   %ebp
  1019a1:	57                   	push   %edi
  1019a2:	56                   	push   %esi
  1019a3:	53                   	push   %ebx
  1019a4:	83 ec 30             	sub    $0x30,%esp
  1019a7:	8b 4c 24 4c          	mov    0x4c(%esp),%ecx
  1019ab:	8b 6c 24 48          	mov    0x48(%esp),%ebp
  1019af:	0f b6 01             	movzbl (%ecx),%eax
  1019b2:	84 c0                	test   %al,%al
  1019b4:	0f 84 75 04 00 00    	je     101e2f <vsnprintf+0x48f>
  1019ba:	31 ff                	xor    %edi,%edi
  1019bc:	eb 15                	jmp    1019d3 <vsnprintf+0x33>
  1019be:	66 90                	xchg   %ax,%ax
  1019c0:	8b 74 24 44          	mov    0x44(%esp),%esi
  1019c4:	83 c1 01             	add    $0x1,%ecx
  1019c7:	88 04 3e             	mov    %al,(%esi,%edi,1)
  1019ca:	89 d7                	mov    %edx,%edi
  1019cc:	0f b6 01             	movzbl (%ecx),%eax
  1019cf:	84 c0                	test   %al,%al
  1019d1:	74 14                	je     1019e7 <vsnprintf+0x47>
  1019d3:	3c 25                	cmp    $0x25,%al
  1019d5:	74 41                	je     101a18 <vsnprintf+0x78>
  1019d7:	8d 57 01             	lea    0x1(%edi),%edx
  1019da:	39 ea                	cmp    %ebp,%edx
  1019dc:	72 e2                	jb     1019c0 <vsnprintf+0x20>
  1019de:	0f b6 01             	movzbl (%ecx),%eax
  1019e1:	89 d7                	mov    %edx,%edi
  1019e3:	84 c0                	test   %al,%al
  1019e5:	75 ec                	jne    1019d3 <vsnprintf+0x33>
  1019e7:	89 f8                	mov    %edi,%eax
  1019e9:	85 ed                	test   %ebp,%ebp
  1019eb:	74 1e                	je     101a0b <vsnprintf+0x6b>
  1019ed:	8b 54 24 44          	mov    0x44(%esp),%edx
  1019f1:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  1019f5:	01 fa                	add    %edi,%edx
  1019f7:	8d 4c 29 ff          	lea    -0x1(%ecx,%ebp,1),%ecx
  1019fb:	39 ef                	cmp    %ebp,%edi
  1019fd:	0f 43 d1             	cmovae %ecx,%edx
  101a00:	89 54 24 44          	mov    %edx,0x44(%esp)
  101a04:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  101a08:	c6 01 00             	movb   $0x0,(%ecx)
  101a0b:	83 c4 30             	add    $0x30,%esp
  101a0e:	5b                   	pop    %ebx
  101a0f:	5e                   	pop    %esi
  101a10:	5f                   	pop    %edi
  101a11:	5d                   	pop    %ebp
  101a12:	c3                   	ret
  101a13:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  101a18:	0f b6 41 01          	movzbl 0x1(%ecx),%eax
  101a1c:	3c 30                	cmp    $0x30,%al
  101a1e:	74 50                	je     101a70 <vsnprintf+0xd0>
  101a20:	c6 04 24 20          	movb   $0x20,(%esp)
  101a24:	8d 71 01             	lea    0x1(%ecx),%esi
  101a27:	8d 50 d0             	lea    -0x30(%eax),%edx
  101a2a:	31 db                	xor    %ebx,%ebx
  101a2c:	80 fa 09             	cmp    $0x9,%dl
  101a2f:	77 21                	ja     101a52 <vsnprintf+0xb2>
  101a31:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  101a38:	83 e8 30             	sub    $0x30,%eax
  101a3b:	8d 14 9b             	lea    (%ebx,%ebx,4),%edx
  101a3e:	83 c6 01             	add    $0x1,%esi
  101a41:	0f be c0             	movsbl %al,%eax
  101a44:	8d 1c 50             	lea    (%eax,%edx,2),%ebx
  101a47:	0f b6 06             	movzbl (%esi),%eax
  101a4a:	8d 50 d0             	lea    -0x30(%eax),%edx
  101a4d:	80 fa 09             	cmp    $0x9,%dl
  101a50:	76 e6                	jbe    101a38 <vsnprintf+0x98>
  101a52:	3c 25                	cmp    $0x25,%al
  101a54:	0f 84 8e 03 00 00    	je     101de8 <vsnprintf+0x448>
  101a5a:	83 e8 58             	sub    $0x58,%eax
  101a5d:	3c 20                	cmp    $0x20,%al
  101a5f:	77 1f                	ja     101a80 <vsnprintf+0xe0>
  101a61:	0f b6 c0             	movzbl %al,%eax
  101a64:	ff 24 85 c0 a4 10 00 	jmp    *0x10a4c0(,%eax,4)
  101a6b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  101a70:	c6 04 24 30          	movb   $0x30,(%esp)
  101a74:	0f b6 41 02          	movzbl 0x2(%ecx),%eax
  101a78:	8d 71 02             	lea    0x2(%ecx),%esi
  101a7b:	eb aa                	jmp    101a27 <vsnprintf+0x87>
  101a7d:	8d 76 00             	lea    0x0(%esi),%esi
  101a80:	8d 57 01             	lea    0x1(%edi),%edx
  101a83:	39 ea                	cmp    %ebp,%edx
  101a85:	73 08                	jae    101a8f <vsnprintf+0xef>
  101a87:	8b 44 24 44          	mov    0x44(%esp),%eax
  101a8b:	c6 04 38 25          	movb   $0x25,(%eax,%edi,1)
  101a8f:	83 c7 02             	add    $0x2,%edi
  101a92:	39 ef                	cmp    %ebp,%edi
  101a94:	0f 83 a2 00 00 00    	jae    101b3c <vsnprintf+0x19c>
  101a9a:	0f b6 06             	movzbl (%esi),%eax
  101a9d:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  101aa1:	88 04 11             	mov    %al,(%ecx,%edx,1)
  101aa4:	e9 93 00 00 00       	jmp    101b3c <vsnprintf+0x19c>
  101aa9:	8b 44 24 50          	mov    0x50(%esp),%eax
  101aad:	8d 54 24 10          	lea    0x10(%esp),%edx
  101ab1:	31 c9                	xor    %ecx,%ecx
  101ab3:	83 c0 04             	add    $0x4,%eax
  101ab6:	89 44 24 04          	mov    %eax,0x4(%esp)
  101aba:	8b 44 24 50          	mov    0x50(%esp),%eax
  101abe:	8b 00                	mov    (%eax),%eax
  101ac0:	85 c0                	test   %eax,%eax
  101ac2:	0f 88 43 03 00 00    	js     101e0b <vsnprintf+0x46b>
  101ac8:	89 4c 24 08          	mov    %ecx,0x8(%esp)
  101acc:	6a 00                	push   $0x0
  101ace:	6a 0a                	push   $0xa
  101ad0:	50                   	push   %eax
  101ad1:	52                   	push   %edx
  101ad2:	e8 c9 fd ff ff       	call   1018a0 <uitoa>
  101ad7:	8b 4c 24 18          	mov    0x18(%esp),%ecx
  101adb:	83 c4 10             	add    $0x10,%esp
  101ade:	01 c8                	add    %ecx,%eax
  101ae0:	39 d8                	cmp    %ebx,%eax
  101ae2:	0f 8d 93 03 00 00    	jge    101e7b <vsnprintf+0x4db>
  101ae8:	01 fb                	add    %edi,%ebx
  101aea:	0f b6 14 24          	movzbl (%esp),%edx
  101aee:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  101af2:	29 c3                	sub    %eax,%ebx
  101af4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  101af8:	83 c7 01             	add    $0x1,%edi
  101afb:	39 ef                	cmp    %ebp,%edi
  101afd:	73 04                	jae    101b03 <vsnprintf+0x163>
  101aff:	88 54 39 ff          	mov    %dl,-0x1(%ecx,%edi,1)
  101b03:	39 df                	cmp    %ebx,%edi
  101b05:	75 f1                	jne    101af8 <vsnprintf+0x158>
  101b07:	85 c0                	test   %eax,%eax
  101b09:	0f 8e 73 03 00 00    	jle    101e82 <vsnprintf+0x4e2>
  101b0f:	8d 3c 18             	lea    (%eax,%ebx,1),%edi
  101b12:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  101b16:	8d 44 24 10          	lea    0x10(%esp),%eax
  101b1a:	29 d8                	sub    %ebx,%eax
  101b1c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  101b20:	83 c3 01             	add    $0x1,%ebx
  101b23:	39 eb                	cmp    %ebp,%ebx
  101b25:	73 09                	jae    101b30 <vsnprintf+0x190>
  101b27:	0f b6 54 18 ff       	movzbl -0x1(%eax,%ebx,1),%edx
  101b2c:	88 54 19 ff          	mov    %dl,-0x1(%ecx,%ebx,1)
  101b30:	39 fb                	cmp    %edi,%ebx
  101b32:	75 ec                	jne    101b20 <vsnprintf+0x180>
  101b34:	8b 44 24 04          	mov    0x4(%esp),%eax
  101b38:	89 44 24 50          	mov    %eax,0x50(%esp)
  101b3c:	8d 4e 01             	lea    0x1(%esi),%ecx
  101b3f:	e9 88 fe ff ff       	jmp    1019cc <vsnprintf+0x2c>
  101b44:	8b 44 24 50          	mov    0x50(%esp),%eax
  101b48:	83 c0 04             	add    $0x4,%eax
  101b4b:	89 44 24 08          	mov    %eax,0x8(%esp)
  101b4f:	31 c0                	xor    %eax,%eax
  101b51:	80 3e 58             	cmpb   $0x58,(%esi)
  101b54:	0f 94 c0             	sete   %al
  101b57:	50                   	push   %eax
  101b58:	6a 10                	push   $0x10
  101b5a:	8b 44 24 58          	mov    0x58(%esp),%eax
  101b5e:	ff 30                	push   (%eax)
  101b60:	8d 44 24 1c          	lea    0x1c(%esp),%eax
  101b64:	89 44 24 10          	mov    %eax,0x10(%esp)
  101b68:	50                   	push   %eax
  101b69:	e8 32 fd ff ff       	call   1018a0 <uitoa>
  101b6e:	83 c4 10             	add    $0x10,%esp
  101b71:	39 d8                	cmp    %ebx,%eax
  101b73:	0f 8d 18 03 00 00    	jge    101e91 <vsnprintf+0x4f1>
  101b79:	01 fb                	add    %edi,%ebx
  101b7b:	0f b6 14 24          	movzbl (%esp),%edx
  101b7f:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  101b83:	29 c3                	sub    %eax,%ebx
  101b85:	8d 76 00             	lea    0x0(%esi),%esi
  101b88:	83 c7 01             	add    $0x1,%edi
  101b8b:	39 ef                	cmp    %ebp,%edi
  101b8d:	73 04                	jae    101b93 <vsnprintf+0x1f3>
  101b8f:	88 54 39 ff          	mov    %dl,-0x1(%ecx,%edi,1)
  101b93:	39 df                	cmp    %ebx,%edi
  101b95:	75 f1                	jne    101b88 <vsnprintf+0x1e8>
  101b97:	85 c0                	test   %eax,%eax
  101b99:	0f 8e 9f 02 00 00    	jle    101e3e <vsnprintf+0x49e>
  101b9f:	8d 3c 18             	lea    (%eax,%ebx,1),%edi
  101ba2:	8b 44 24 04          	mov    0x4(%esp),%eax
  101ba6:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  101baa:	29 d8                	sub    %ebx,%eax
  101bac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  101bb0:	83 c3 01             	add    $0x1,%ebx
  101bb3:	39 eb                	cmp    %ebp,%ebx
  101bb5:	73 09                	jae    101bc0 <vsnprintf+0x220>
  101bb7:	0f b6 54 18 ff       	movzbl -0x1(%eax,%ebx,1),%edx
  101bbc:	88 54 19 ff          	mov    %dl,-0x1(%ecx,%ebx,1)
  101bc0:	39 fb                	cmp    %edi,%ebx
  101bc2:	75 ec                	jne    101bb0 <vsnprintf+0x210>
  101bc4:	8b 44 24 08          	mov    0x8(%esp),%eax
  101bc8:	89 44 24 50          	mov    %eax,0x50(%esp)
  101bcc:	e9 6b ff ff ff       	jmp    101b3c <vsnprintf+0x19c>
  101bd1:	8b 44 24 50          	mov    0x50(%esp),%eax
  101bd5:	83 c0 04             	add    $0x4,%eax
  101bd8:	89 04 24             	mov    %eax,(%esp)
  101bdb:	8b 44 24 50          	mov    0x50(%esp),%eax
  101bdf:	8b 10                	mov    (%eax),%edx
  101be1:	8d 47 01             	lea    0x1(%edi),%eax
  101be4:	39 e8                	cmp    %ebp,%eax
  101be6:	73 08                	jae    101bf0 <vsnprintf+0x250>
  101be8:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  101bec:	c6 04 39 30          	movb   $0x30,(%ecx,%edi,1)
  101bf0:	8d 5f 02             	lea    0x2(%edi),%ebx
  101bf3:	39 eb                	cmp    %ebp,%ebx
  101bf5:	0f 82 03 02 00 00    	jb     101dfe <vsnprintf+0x45e>
  101bfb:	6a 00                	push   $0x0
  101bfd:	6a 10                	push   $0x10
  101bff:	52                   	push   %edx
  101c00:	8d 44 24 1c          	lea    0x1c(%esp),%eax
  101c04:	89 44 24 10          	mov    %eax,0x10(%esp)
  101c08:	50                   	push   %eax
  101c09:	e8 92 fc ff ff       	call   1018a0 <uitoa>
  101c0e:	83 c4 10             	add    $0x10,%esp
  101c11:	89 c1                	mov    %eax,%ecx
  101c13:	83 f8 07             	cmp    $0x7,%eax
  101c16:	0f 8f 45 02 00 00    	jg     101e61 <vsnprintf+0x4c1>
  101c1c:	83 c7 0a             	add    $0xa,%edi
  101c1f:	29 c7                	sub    %eax,%edi
  101c21:	8b 44 24 44          	mov    0x44(%esp),%eax
  101c25:	8d 76 00             	lea    0x0(%esi),%esi
  101c28:	83 c3 01             	add    $0x1,%ebx
  101c2b:	39 eb                	cmp    %ebp,%ebx
  101c2d:	73 05                	jae    101c34 <vsnprintf+0x294>
  101c2f:	c6 44 18 ff 30       	movb   $0x30,-0x1(%eax,%ebx,1)
  101c34:	39 fb                	cmp    %edi,%ebx
  101c36:	75 f0                	jne    101c28 <vsnprintf+0x288>
  101c38:	85 c9                	test   %ecx,%ecx
  101c3a:	0f 8e 28 02 00 00    	jle    101e68 <vsnprintf+0x4c8>
  101c40:	8b 54 24 44          	mov    0x44(%esp),%edx
  101c44:	89 7c 24 08          	mov    %edi,0x8(%esp)
  101c48:	8d 5f 01             	lea    0x1(%edi),%ebx
  101c4b:	31 c0                	xor    %eax,%eax
  101c4d:	89 74 24 0c          	mov    %esi,0xc(%esp)
  101c51:	01 fa                	add    %edi,%edx
  101c53:	8b 7c 24 04          	mov    0x4(%esp),%edi
  101c57:	89 d6                	mov    %edx,%esi
  101c59:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  101c60:	8d 14 03             	lea    (%ebx,%eax,1),%edx
  101c63:	39 ea                	cmp    %ebp,%edx
  101c65:	73 07                	jae    101c6e <vsnprintf+0x2ce>
  101c67:	0f b6 14 07          	movzbl (%edi,%eax,1),%edx
  101c6b:	88 14 06             	mov    %dl,(%esi,%eax,1)
  101c6e:	83 c0 01             	add    $0x1,%eax
  101c71:	39 c1                	cmp    %eax,%ecx
  101c73:	7f eb                	jg     101c60 <vsnprintf+0x2c0>
  101c75:	8b 7c 24 08          	mov    0x8(%esp),%edi
  101c79:	8b 04 24             	mov    (%esp),%eax
  101c7c:	8b 74 24 0c          	mov    0xc(%esp),%esi
  101c80:	89 44 24 50          	mov    %eax,0x50(%esp)
  101c84:	01 cf                	add    %ecx,%edi
  101c86:	e9 b1 fe ff ff       	jmp    101b3c <vsnprintf+0x19c>
  101c8b:	8d 57 01             	lea    0x1(%edi),%edx
  101c8e:	39 ea                	cmp    %ebp,%edx
  101c90:	73 0d                	jae    101c9f <vsnprintf+0x2ff>
  101c92:	8b 44 24 50          	mov    0x50(%esp),%eax
  101c96:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  101c9a:	8b 00                	mov    (%eax),%eax
  101c9c:	88 04 39             	mov    %al,(%ecx,%edi,1)
  101c9f:	83 44 24 50 04       	addl   $0x4,0x50(%esp)
  101ca4:	89 d7                	mov    %edx,%edi
  101ca6:	e9 91 fe ff ff       	jmp    101b3c <vsnprintf+0x19c>
  101cab:	8b 44 24 50          	mov    0x50(%esp),%eax
  101caf:	83 c0 04             	add    $0x4,%eax
  101cb2:	89 44 24 08          	mov    %eax,0x8(%esp)
  101cb6:	6a 00                	push   $0x0
  101cb8:	6a 0a                	push   $0xa
  101cba:	8b 44 24 58          	mov    0x58(%esp),%eax
  101cbe:	ff 30                	push   (%eax)
  101cc0:	8d 44 24 1c          	lea    0x1c(%esp),%eax
  101cc4:	89 44 24 10          	mov    %eax,0x10(%esp)
  101cc8:	50                   	push   %eax
  101cc9:	e8 d2 fb ff ff       	call   1018a0 <uitoa>
  101cce:	83 c4 10             	add    $0x10,%esp
  101cd1:	39 d8                	cmp    %ebx,%eax
  101cd3:	0f 8d 9b 01 00 00    	jge    101e74 <vsnprintf+0x4d4>
  101cd9:	01 fb                	add    %edi,%ebx
  101cdb:	0f b6 14 24          	movzbl (%esp),%edx
  101cdf:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  101ce3:	29 c3                	sub    %eax,%ebx
  101ce5:	8d 76 00             	lea    0x0(%esi),%esi
  101ce8:	83 c7 01             	add    $0x1,%edi
  101ceb:	39 ef                	cmp    %ebp,%edi
  101ced:	73 04                	jae    101cf3 <vsnprintf+0x353>
  101cef:	88 54 39 ff          	mov    %dl,-0x1(%ecx,%edi,1)
  101cf3:	39 df                	cmp    %ebx,%edi
  101cf5:	75 f1                	jne    101ce8 <vsnprintf+0x348>
  101cf7:	85 c0                	test   %eax,%eax
  101cf9:	0f 8e 3f 01 00 00    	jle    101e3e <vsnprintf+0x49e>
  101cff:	8d 3c 18             	lea    (%eax,%ebx,1),%edi
  101d02:	8b 44 24 04          	mov    0x4(%esp),%eax
  101d06:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  101d0a:	29 d8                	sub    %ebx,%eax
  101d0c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  101d10:	83 c3 01             	add    $0x1,%ebx
  101d13:	39 eb                	cmp    %ebp,%ebx
  101d15:	73 09                	jae    101d20 <vsnprintf+0x380>
  101d17:	0f b6 54 18 ff       	movzbl -0x1(%eax,%ebx,1),%edx
  101d1c:	88 54 19 ff          	mov    %dl,-0x1(%ecx,%ebx,1)
  101d20:	39 fb                	cmp    %edi,%ebx
  101d22:	75 ec                	jne    101d10 <vsnprintf+0x370>
  101d24:	8b 44 24 08          	mov    0x8(%esp),%eax
  101d28:	89 44 24 50          	mov    %eax,0x50(%esp)
  101d2c:	e9 0b fe ff ff       	jmp    101b3c <vsnprintf+0x19c>
  101d31:	8b 44 24 50          	mov    0x50(%esp),%eax
  101d35:	83 c0 04             	add    $0x4,%eax
  101d38:	89 44 24 04          	mov    %eax,0x4(%esp)
  101d3c:	8b 44 24 50          	mov    0x50(%esp),%eax
  101d40:	8b 10                	mov    (%eax),%edx
  101d42:	85 d2                	test   %edx,%edx
  101d44:	0f 84 d6 00 00 00    	je     101e20 <vsnprintf+0x480>
  101d4a:	0f b6 0a             	movzbl (%edx),%ecx
  101d4d:	84 c9                	test   %cl,%cl
  101d4f:	0f 84 f8 00 00 00    	je     101e4d <vsnprintf+0x4ad>
  101d55:	31 c0                	xor    %eax,%eax
  101d57:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  101d5e:	00 
  101d5f:	90                   	nop
  101d60:	83 c0 01             	add    $0x1,%eax
  101d63:	80 3c 02 00          	cmpb   $0x0,(%edx,%eax,1)
  101d67:	75 f7                	jne    101d60 <vsnprintf+0x3c0>
  101d69:	89 04 24             	mov    %eax,(%esp)
  101d6c:	39 d8                	cmp    %ebx,%eax
  101d6e:	7d 4e                	jge    101dbe <vsnprintf+0x41e>
  101d70:	8b 0c 24             	mov    (%esp),%ecx
  101d73:	89 7c 24 08          	mov    %edi,0x8(%esp)
  101d77:	89 f8                	mov    %edi,%eax
  101d79:	29 f9                	sub    %edi,%ecx
  101d7b:	89 d7                	mov    %edx,%edi
  101d7d:	8d 76 00             	lea    0x0(%esi),%esi
  101d80:	83 c0 01             	add    $0x1,%eax
  101d83:	39 e8                	cmp    %ebp,%eax
  101d85:	73 09                	jae    101d90 <vsnprintf+0x3f0>
  101d87:	8b 54 24 44          	mov    0x44(%esp),%edx
  101d8b:	c6 44 02 ff 20       	movb   $0x20,-0x1(%edx,%eax,1)
  101d90:	8d 14 01             	lea    (%ecx,%eax,1),%edx
  101d93:	39 d3                	cmp    %edx,%ebx
  101d95:	7f e9                	jg     101d80 <vsnprintf+0x3e0>
  101d97:	8b 0c 24             	mov    (%esp),%ecx
  101d9a:	8d 43 ff             	lea    -0x1(%ebx),%eax
  101d9d:	89 fa                	mov    %edi,%edx
  101d9f:	8b 7c 24 08          	mov    0x8(%esp),%edi
  101da3:	29 c8                	sub    %ecx,%eax
  101da5:	39 d9                	cmp    %ebx,%ecx
  101da7:	b9 00 00 00 00       	mov    $0x0,%ecx
  101dac:	0f 4d c1             	cmovge %ecx,%eax
  101daf:	0f b6 0a             	movzbl (%edx),%ecx
  101db2:	8d 7c 07 01          	lea    0x1(%edi,%eax,1),%edi
  101db6:	84 c9                	test   %cl,%cl
  101db8:	0f 84 76 fd ff ff    	je     101b34 <vsnprintf+0x194>
  101dbe:	8b 44 24 44          	mov    0x44(%esp),%eax
  101dc2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  101dc8:	83 c7 01             	add    $0x1,%edi
  101dcb:	39 ef                	cmp    %ebp,%edi
  101dcd:	73 07                	jae    101dd6 <vsnprintf+0x436>
  101dcf:	88 4c 38 ff          	mov    %cl,-0x1(%eax,%edi,1)
  101dd3:	83 c2 01             	add    $0x1,%edx
  101dd6:	0f b6 0a             	movzbl (%edx),%ecx
  101dd9:	84 c9                	test   %cl,%cl
  101ddb:	75 eb                	jne    101dc8 <vsnprintf+0x428>
  101ddd:	e9 52 fd ff ff       	jmp    101b34 <vsnprintf+0x194>
  101de2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  101de8:	8d 47 01             	lea    0x1(%edi),%eax
  101deb:	39 e8                	cmp    %ebp,%eax
  101ded:	73 08                	jae    101df7 <vsnprintf+0x457>
  101def:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  101df3:	c6 04 39 25          	movb   $0x25,(%ecx,%edi,1)
  101df7:	89 c7                	mov    %eax,%edi
  101df9:	e9 3e fd ff ff       	jmp    101b3c <vsnprintf+0x19c>
  101dfe:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  101e02:	c6 04 01 78          	movb   $0x78,(%ecx,%eax,1)
  101e06:	e9 f0 fd ff ff       	jmp    101bfb <vsnprintf+0x25b>
  101e0b:	c6 44 24 10 2d       	movb   $0x2d,0x10(%esp)
  101e10:	f7 d8                	neg    %eax
  101e12:	8d 54 24 11          	lea    0x11(%esp),%edx
  101e16:	b9 01 00 00 00       	mov    $0x1,%ecx
  101e1b:	e9 a8 fc ff ff       	jmp    101ac8 <vsnprintf+0x128>
  101e20:	ba 75 91 10 00       	mov    $0x109175,%edx
  101e25:	b9 28 00 00 00       	mov    $0x28,%ecx
  101e2a:	e9 26 ff ff ff       	jmp    101d55 <vsnprintf+0x3b5>
  101e2f:	31 c0                	xor    %eax,%eax
  101e31:	85 ed                	test   %ebp,%ebp
  101e33:	0f 85 cb fb ff ff    	jne    101a04 <vsnprintf+0x64>
  101e39:	e9 cd fb ff ff       	jmp    101a0b <vsnprintf+0x6b>
  101e3e:	8b 44 24 08          	mov    0x8(%esp),%eax
  101e42:	89 df                	mov    %ebx,%edi
  101e44:	89 44 24 50          	mov    %eax,0x50(%esp)
  101e48:	e9 ef fc ff ff       	jmp    101b3c <vsnprintf+0x19c>
  101e4d:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  101e54:	85 db                	test   %ebx,%ebx
  101e56:	0f 8f 14 ff ff ff    	jg     101d70 <vsnprintf+0x3d0>
  101e5c:	e9 d3 fc ff ff       	jmp    101b34 <vsnprintf+0x194>
  101e61:	89 df                	mov    %ebx,%edi
  101e63:	e9 d8 fd ff ff       	jmp    101c40 <vsnprintf+0x2a0>
  101e68:	8b 04 24             	mov    (%esp),%eax
  101e6b:	89 44 24 50          	mov    %eax,0x50(%esp)
  101e6f:	e9 c8 fc ff ff       	jmp    101b3c <vsnprintf+0x19c>
  101e74:	89 fb                	mov    %edi,%ebx
  101e76:	e9 7c fe ff ff       	jmp    101cf7 <vsnprintf+0x357>
  101e7b:	89 fb                	mov    %edi,%ebx
  101e7d:	e9 85 fc ff ff       	jmp    101b07 <vsnprintf+0x167>
  101e82:	8b 44 24 04          	mov    0x4(%esp),%eax
  101e86:	89 df                	mov    %ebx,%edi
  101e88:	89 44 24 50          	mov    %eax,0x50(%esp)
  101e8c:	e9 ab fc ff ff       	jmp    101b3c <vsnprintf+0x19c>
  101e91:	89 fb                	mov    %edi,%ebx
  101e93:	e9 ff fc ff ff       	jmp    101b97 <vsnprintf+0x1f7>
  101e98:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  101e9f:	00 

00101ea0 <snprintf>:
  101ea0:	8d 44 24 10          	lea    0x10(%esp),%eax
  101ea4:	50                   	push   %eax
  101ea5:	ff 74 24 10          	push   0x10(%esp)
  101ea9:	ff 74 24 10          	push   0x10(%esp)
  101ead:	ff 74 24 10          	push   0x10(%esp)
  101eb1:	e8 ea fa ff ff       	call   1019a0 <vsnprintf>
  101eb6:	83 c4 10             	add    $0x10,%esp
  101eb9:	c3                   	ret
  101eba:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00101ec0 <__udivdi3>:
  101ec0:	57                   	push   %edi
  101ec1:	56                   	push   %esi
  101ec2:	53                   	push   %ebx
  101ec3:	83 ec 18             	sub    $0x18,%esp
  101ec6:	8b 44 24 28          	mov    0x28(%esp),%eax
  101eca:	8b 4c 24 34          	mov    0x34(%esp),%ecx
  101ece:	c7 44 24 10 00 00 00 	movl   $0x0,0x10(%esp)
  101ed5:	00 
  101ed6:	8b 54 24 2c          	mov    0x2c(%esp),%edx
  101eda:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
  101ee1:	00 
  101ee2:	89 04 24             	mov    %eax,(%esp)
  101ee5:	8b 44 24 30          	mov    0x30(%esp),%eax
  101ee9:	89 54 24 04          	mov    %edx,0x4(%esp)
  101eed:	89 44 24 08          	mov    %eax,0x8(%esp)
  101ef1:	09 c8                	or     %ecx,%eax
  101ef3:	89 4c 24 0c          	mov    %ecx,0xc(%esp)
  101ef7:	74 62                	je     101f5b <__udivdi3+0x9b>
  101ef9:	b9 3f 00 00 00       	mov    $0x3f,%ecx
  101efe:	31 c0                	xor    %eax,%eax
  101f00:	31 d2                	xor    %edx,%edx
  101f02:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  101f08:	8b 7c 24 04          	mov    0x4(%esp),%edi
  101f0c:	8b 34 24             	mov    (%esp),%esi
  101f0f:	0f ad fe             	shrd   %cl,%edi,%esi
  101f12:	d3 ef                	shr    %cl,%edi
  101f14:	f6 c1 20             	test   $0x20,%cl
  101f17:	0f 45 f7             	cmovne %edi,%esi
  101f1a:	0f a4 c2 01          	shld   $0x1,%eax,%edx
  101f1e:	01 c0                	add    %eax,%eax
  101f20:	31 ff                	xor    %edi,%edi
  101f22:	09 fa                	or     %edi,%edx
  101f24:	8b 7c 24 0c          	mov    0xc(%esp),%edi
  101f28:	83 e6 01             	and    $0x1,%esi
  101f2b:	89 d3                	mov    %edx,%ebx
  101f2d:	09 f0                	or     %esi,%eax
  101f2f:	8b 74 24 08          	mov    0x8(%esp),%esi
  101f33:	39 f0                	cmp    %esi,%eax
  101f35:	19 fb                	sbb    %edi,%ebx
  101f37:	72 1d                	jb     101f56 <__udivdi3+0x96>
  101f39:	29 f0                	sub    %esi,%eax
  101f3b:	19 fa                	sbb    %edi,%edx
  101f3d:	89 cf                	mov    %ecx,%edi
  101f3f:	c1 ef 05             	shr    $0x5,%edi
  101f42:	83 e7 01             	and    $0x1,%edi
  101f45:	89 fe                	mov    %edi,%esi
  101f47:	d3 e7                	shl    %cl,%edi
  101f49:	09 7c 24 14          	or     %edi,0x14(%esp)
  101f4d:	83 f6 01             	xor    $0x1,%esi
  101f50:	d3 e6                	shl    %cl,%esi
  101f52:	09 74 24 10          	or     %esi,0x10(%esp)
  101f56:	83 e9 01             	sub    $0x1,%ecx
  101f59:	73 ad                	jae    101f08 <__udivdi3+0x48>
  101f5b:	8b 44 24 10          	mov    0x10(%esp),%eax
  101f5f:	8b 54 24 14          	mov    0x14(%esp),%edx
  101f63:	83 c4 18             	add    $0x18,%esp
  101f66:	5b                   	pop    %ebx
  101f67:	5e                   	pop    %esi
  101f68:	5f                   	pop    %edi
  101f69:	c3                   	ret
  101f6a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00101f70 <__umoddi3>:
  101f70:	57                   	push   %edi
  101f71:	56                   	push   %esi
  101f72:	53                   	push   %ebx
  101f73:	83 ec 10             	sub    $0x10,%esp
  101f76:	8b 44 24 20          	mov    0x20(%esp),%eax
  101f7a:	8b 54 24 24          	mov    0x24(%esp),%edx
  101f7e:	89 04 24             	mov    %eax,(%esp)
  101f81:	8b 44 24 28          	mov    0x28(%esp),%eax
  101f85:	89 54 24 04          	mov    %edx,0x4(%esp)
  101f89:	8b 54 24 2c          	mov    0x2c(%esp),%edx
  101f8d:	89 c6                	mov    %eax,%esi
  101f8f:	89 44 24 08          	mov    %eax,0x8(%esp)
  101f93:	31 c0                	xor    %eax,%eax
  101f95:	89 d7                	mov    %edx,%edi
  101f97:	89 f1                	mov    %esi,%ecx
  101f99:	89 54 24 0c          	mov    %edx,0xc(%esp)
  101f9d:	31 d2                	xor    %edx,%edx
  101f9f:	09 f9                	or     %edi,%ecx
  101fa1:	74 47                	je     101fea <__umoddi3+0x7a>
  101fa3:	b9 3f 00 00 00       	mov    $0x3f,%ecx
  101fa8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  101faf:	00 
  101fb0:	8b 7c 24 04          	mov    0x4(%esp),%edi
  101fb4:	8b 34 24             	mov    (%esp),%esi
  101fb7:	0f ad fe             	shrd   %cl,%edi,%esi
  101fba:	d3 ef                	shr    %cl,%edi
  101fbc:	f6 c1 20             	test   $0x20,%cl
  101fbf:	0f 45 f7             	cmovne %edi,%esi
  101fc2:	0f a4 c2 01          	shld   $0x1,%eax,%edx
  101fc6:	01 c0                	add    %eax,%eax
  101fc8:	31 ff                	xor    %edi,%edi
  101fca:	09 fa                	or     %edi,%edx
  101fcc:	8b 7c 24 0c          	mov    0xc(%esp),%edi
  101fd0:	83 e6 01             	and    $0x1,%esi
  101fd3:	89 d3                	mov    %edx,%ebx
  101fd5:	09 f0                	or     %esi,%eax
  101fd7:	8b 74 24 08          	mov    0x8(%esp),%esi
  101fdb:	39 f0                	cmp    %esi,%eax
  101fdd:	19 fb                	sbb    %edi,%ebx
  101fdf:	72 04                	jb     101fe5 <__umoddi3+0x75>
  101fe1:	29 f0                	sub    %esi,%eax
  101fe3:	19 fa                	sbb    %edi,%edx
  101fe5:	83 e9 01             	sub    $0x1,%ecx
  101fe8:	73 c6                	jae    101fb0 <__umoddi3+0x40>
  101fea:	83 c4 10             	add    $0x10,%esp
  101fed:	5b                   	pop    %ebx
  101fee:	5e                   	pop    %esi
  101fef:	5f                   	pop    %edi
  101ff0:	c3                   	ret
  101ff1:	66 90                	xchg   %ax,%ax
  101ff3:	66 90                	xchg   %ax,%ax
  101ff5:	66 90                	xchg   %ax,%ax
  101ff7:	66 90                	xchg   %ax,%ax
  101ff9:	66 90                	xchg   %ax,%ax
  101ffb:	66 90                	xchg   %ax,%ax
  101ffd:	66 90                	xchg   %ax,%ax
  101fff:	90                   	nop

00102000 <gdt_init>:
  102000:	83 ec 10             	sub    $0x10,%esp
  102003:	c7 05 80 00 11 00 00 	movl   $0x0,0x110080
  10200a:	00 00 00 
  10200d:	6a 68                	push   $0x68
  10200f:	6a 00                	push   $0x0
  102011:	68 00 00 11 00       	push   $0x110000
  102016:	c7 05 84 00 11 00 00 	movl   $0x0,0x110084
  10201d:	00 00 00 
  102020:	c7 05 88 00 11 00 ff 	movl   $0xffff,0x110088
  102027:	ff 00 00 
  10202a:	c7 05 8c 00 11 00 00 	movl   $0xcf9a00,0x11008c
  102031:	9a cf 00 
  102034:	c7 05 90 00 11 00 ff 	movl   $0xffff,0x110090
  10203b:	ff 00 00 
  10203e:	c7 05 94 00 11 00 00 	movl   $0xcf9200,0x110094
  102045:	92 cf 00 
  102048:	c7 05 98 00 11 00 ff 	movl   $0xffff,0x110098
  10204f:	ff 00 00 
  102052:	c7 05 9c 00 11 00 00 	movl   $0xcffa00,0x11009c
  102059:	fa cf 00 
  10205c:	c7 05 a0 00 11 00 ff 	movl   $0xffff,0x1100a0
  102063:	ff 00 00 
  102066:	c7 05 a4 00 11 00 00 	movl   $0xcff200,0x1100a4
  10206d:	f2 cf 00 
  102070:	e8 eb f4 ff ff       	call   101560 <memset>
  102075:	b8 00 00 11 00       	mov    $0x110000,%eax
  10207a:	ba 68 00 00 00       	mov    $0x68,%edx
  10207f:	b9 67 00 00 00       	mov    $0x67,%ecx
  102084:	66 89 15 66 00 11 00 	mov    %dx,0x110066
  10208b:	89 c2                	mov    %eax,%edx
  10208d:	c1 ea 10             	shr    $0x10,%edx
  102090:	66 89 0d a8 00 11 00 	mov    %cx,0x1100a8
  102097:	b9 2f 00 00 00       	mov    $0x2f,%ecx
  10209c:	66 a3 aa 00 11 00    	mov    %ax,0x1100aa
  1020a2:	c1 e8 18             	shr    $0x18,%eax
  1020a5:	88 15 ac 00 11 00    	mov    %dl,0x1100ac
  1020ab:	ba 89 00 00 00       	mov    $0x89,%edx
  1020b0:	c7 04 24 68 00 11 00 	movl   $0x110068,(%esp)
  1020b7:	c7 05 08 00 11 00 10 	movl   $0x10,0x110008
  1020be:	00 00 00 
  1020c1:	c7 05 04 00 11 00 00 	movl   $0x0,0x110004
  1020c8:	00 00 00 
  1020cb:	66 89 15 ad 00 11 00 	mov    %dx,0x1100ad
  1020d2:	a2 af 00 11 00       	mov    %al,0x1100af
  1020d7:	66 89 0d 68 00 11 00 	mov    %cx,0x110068
  1020de:	c7 05 6a 00 11 00 80 	movl   $0x110080,0x11006a
  1020e5:	00 11 00 
  1020e8:	e8 26 ef ff ff       	call   101013 <gdt_flush>
  1020ed:	83 c4 1c             	add    $0x1c,%esp
  1020f0:	e9 3b ef ff ff       	jmp    101030 <tss_flush>
  1020f5:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1020fc:	00 
  1020fd:	8d 76 00             	lea    0x0(%esi),%esi

00102100 <tss_set_kernel_stack>:
  102100:	8b 44 24 04          	mov    0x4(%esp),%eax
  102104:	a3 04 00 11 00       	mov    %eax,0x110004
  102109:	c3                   	ret
  10210a:	66 90                	xchg   %ax,%ax
  10210c:	66 90                	xchg   %ax,%ax
  10210e:	66 90                	xchg   %ax,%ax

00102110 <idt_init>:
  102110:	56                   	push   %esi
  102111:	53                   	push   %ebx
  102112:	83 ec 08             	sub    $0x8,%esp
  102115:	68 00 08 00 00       	push   $0x800
  10211a:	6a 00                	push   $0x0
  10211c:	68 a0 01 11 00       	push   $0x1101a0
  102121:	e8 3a f4 ff ff       	call   101560 <memset>
  102126:	83 c4 0c             	add    $0xc,%esp
  102129:	6a 40                	push   $0x40
  10212b:	6a 00                	push   $0x0
  10212d:	68 40 01 11 00       	push   $0x110140
  102132:	e8 29 f4 ff ff       	call   101560 <memset>
  102137:	83 c4 0c             	add    $0xc,%esp
  10213a:	68 80 00 00 00       	push   $0x80
  10213f:	6a 00                	push   $0x0
  102141:	68 c0 00 11 00       	push   $0x1100c0
  102146:	e8 15 f4 ff ff       	call   101560 <memset>
  10214b:	e4 21                	in     $0x21,%al
  10214d:	89 c6                	mov    %eax,%esi
  10214f:	e4 a1                	in     $0xa1,%al
  102151:	b9 11 00 00 00       	mov    $0x11,%ecx
  102156:	89 c3                	mov    %eax,%ebx
  102158:	89 c8                	mov    %ecx,%eax
  10215a:	e6 20                	out    %al,$0x20
  10215c:	31 d2                	xor    %edx,%edx
  10215e:	89 d0                	mov    %edx,%eax
  102160:	e6 80                	out    %al,$0x80
  102162:	89 c8                	mov    %ecx,%eax
  102164:	e6 a0                	out    %al,$0xa0
  102166:	89 d0                	mov    %edx,%eax
  102168:	e6 80                	out    %al,$0x80
  10216a:	b8 20 00 00 00       	mov    $0x20,%eax
  10216f:	e6 21                	out    %al,$0x21
  102171:	89 d0                	mov    %edx,%eax
  102173:	e6 80                	out    %al,$0x80
  102175:	b8 28 00 00 00       	mov    $0x28,%eax
  10217a:	e6 a1                	out    %al,$0xa1
  10217c:	89 d0                	mov    %edx,%eax
  10217e:	e6 80                	out    %al,$0x80
  102180:	b8 04 00 00 00       	mov    $0x4,%eax
  102185:	e6 21                	out    %al,$0x21
  102187:	89 d0                	mov    %edx,%eax
  102189:	e6 80                	out    %al,$0x80
  10218b:	b8 02 00 00 00       	mov    $0x2,%eax
  102190:	e6 a1                	out    %al,$0xa1
  102192:	89 d0                	mov    %edx,%eax
  102194:	e6 80                	out    %al,$0x80
  102196:	b9 01 00 00 00       	mov    $0x1,%ecx
  10219b:	89 c8                	mov    %ecx,%eax
  10219d:	e6 21                	out    %al,$0x21
  10219f:	89 d0                	mov    %edx,%eax
  1021a1:	e6 80                	out    %al,$0x80
  1021a3:	89 c8                	mov    %ecx,%eax
  1021a5:	e6 a1                	out    %al,$0xa1
  1021a7:	89 d0                	mov    %edx,%eax
  1021a9:	e6 80                	out    %al,$0x80
  1021ab:	89 f0                	mov    %esi,%eax
  1021ad:	e6 21                	out    %al,$0x21
  1021af:	89 d8                	mov    %ebx,%eax
  1021b1:	e6 a1                	out    %al,$0xa1
  1021b3:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  1021b8:	e6 21                	out    %al,$0x21
  1021ba:	e6 a1                	out    %al,$0xa1
  1021bc:	c7 05 a2 01 11 00 08 	movl   $0x8e000008,0x1101a2
  1021c3:	00 00 8e 
  1021c6:	b8 86 10 10 00       	mov    $0x101086,%eax
  1021cb:	66 a3 a0 01 11 00    	mov    %ax,0x1101a0
  1021d1:	c1 e8 10             	shr    $0x10,%eax
  1021d4:	66 a3 a6 01 11 00    	mov    %ax,0x1101a6
  1021da:	b8 8f 10 10 00       	mov    $0x10108f,%eax
  1021df:	66 a3 a8 01 11 00    	mov    %ax,0x1101a8
  1021e5:	c1 e8 10             	shr    $0x10,%eax
  1021e8:	66 a3 ae 01 11 00    	mov    %ax,0x1101ae
  1021ee:	b8 98 10 10 00       	mov    $0x101098,%eax
  1021f3:	66 a3 b0 01 11 00    	mov    %ax,0x1101b0
  1021f9:	c1 e8 10             	shr    $0x10,%eax
  1021fc:	66 a3 b6 01 11 00    	mov    %ax,0x1101b6
  102202:	b8 a1 10 10 00       	mov    $0x1010a1,%eax
  102207:	66 a3 b8 01 11 00    	mov    %ax,0x1101b8
  10220d:	c1 e8 10             	shr    $0x10,%eax
  102210:	66 a3 be 01 11 00    	mov    %ax,0x1101be
  102216:	b8 aa 10 10 00       	mov    $0x1010aa,%eax
  10221b:	66 a3 c0 01 11 00    	mov    %ax,0x1101c0
  102221:	c1 e8 10             	shr    $0x10,%eax
  102224:	66 a3 c6 01 11 00    	mov    %ax,0x1101c6
  10222a:	b8 b3 10 10 00       	mov    $0x1010b3,%eax
  10222f:	66 a3 c8 01 11 00    	mov    %ax,0x1101c8
  102235:	c1 e8 10             	shr    $0x10,%eax
  102238:	66 a3 ce 01 11 00    	mov    %ax,0x1101ce
  10223e:	b8 bc 10 10 00       	mov    $0x1010bc,%eax
  102243:	66 a3 d0 01 11 00    	mov    %ax,0x1101d0
  102249:	c1 e8 10             	shr    $0x10,%eax
  10224c:	66 a3 d6 01 11 00    	mov    %ax,0x1101d6
  102252:	b8 c5 10 10 00       	mov    $0x1010c5,%eax
  102257:	66 a3 d8 01 11 00    	mov    %ax,0x1101d8
  10225d:	c1 e8 10             	shr    $0x10,%eax
  102260:	66 a3 de 01 11 00    	mov    %ax,0x1101de
  102266:	b8 ce 10 10 00       	mov    $0x1010ce,%eax
  10226b:	66 a3 e0 01 11 00    	mov    %ax,0x1101e0
  102271:	c1 e8 10             	shr    $0x10,%eax
  102274:	66 a3 e6 01 11 00    	mov    %ax,0x1101e6
  10227a:	b8 d5 10 10 00       	mov    $0x1010d5,%eax
  10227f:	66 a3 e8 01 11 00    	mov    %ax,0x1101e8
  102285:	c1 e8 10             	shr    $0x10,%eax
  102288:	66 a3 ee 01 11 00    	mov    %ax,0x1101ee
  10228e:	b8 de 10 10 00       	mov    $0x1010de,%eax
  102293:	66 a3 f0 01 11 00    	mov    %ax,0x1101f0
  102299:	c1 e8 10             	shr    $0x10,%eax
  10229c:	c7 05 aa 01 11 00 08 	movl   $0x8e000008,0x1101aa
  1022a3:	00 00 8e 
  1022a6:	c7 05 b2 01 11 00 08 	movl   $0x8e000008,0x1101b2
  1022ad:	00 00 8e 
  1022b0:	c7 05 ba 01 11 00 08 	movl   $0x8e000008,0x1101ba
  1022b7:	00 00 8e 
  1022ba:	c7 05 c2 01 11 00 08 	movl   $0x8e000008,0x1101c2
  1022c1:	00 00 8e 
  1022c4:	c7 05 ca 01 11 00 08 	movl   $0x8e000008,0x1101ca
  1022cb:	00 00 8e 
  1022ce:	c7 05 d2 01 11 00 08 	movl   $0x8e000008,0x1101d2
  1022d5:	00 00 8e 
  1022d8:	c7 05 da 01 11 00 08 	movl   $0x8e000008,0x1101da
  1022df:	00 00 8e 
  1022e2:	c7 05 e2 01 11 00 08 	movl   $0x8e000008,0x1101e2
  1022e9:	00 00 8e 
  1022ec:	c7 05 ea 01 11 00 08 	movl   $0x8e000008,0x1101ea
  1022f3:	00 00 8e 
  1022f6:	c7 05 f2 01 11 00 08 	movl   $0x8e000008,0x1101f2
  1022fd:	00 00 8e 
  102300:	66 a3 f6 01 11 00    	mov    %ax,0x1101f6
  102306:	b8 e5 10 10 00       	mov    $0x1010e5,%eax
  10230b:	66 a3 f8 01 11 00    	mov    %ax,0x1101f8
  102311:	c1 e8 10             	shr    $0x10,%eax
  102314:	66 a3 fe 01 11 00    	mov    %ax,0x1101fe
  10231a:	b8 ec 10 10 00       	mov    $0x1010ec,%eax
  10231f:	66 a3 00 02 11 00    	mov    %ax,0x110200
  102325:	c1 e8 10             	shr    $0x10,%eax
  102328:	66 a3 06 02 11 00    	mov    %ax,0x110206
  10232e:	b8 f3 10 10 00       	mov    $0x1010f3,%eax
  102333:	66 a3 08 02 11 00    	mov    %ax,0x110208
  102339:	c1 e8 10             	shr    $0x10,%eax
  10233c:	66 a3 0e 02 11 00    	mov    %ax,0x11020e
  102342:	b8 fa 10 10 00       	mov    $0x1010fa,%eax
  102347:	66 a3 10 02 11 00    	mov    %ax,0x110210
  10234d:	c1 e8 10             	shr    $0x10,%eax
  102350:	66 a3 16 02 11 00    	mov    %ax,0x110216
  102356:	b8 01 11 10 00       	mov    $0x101101,%eax
  10235b:	66 a3 18 02 11 00    	mov    %ax,0x110218
  102361:	c1 e8 10             	shr    $0x10,%eax
  102364:	66 a3 1e 02 11 00    	mov    %ax,0x11021e
  10236a:	b8 0a 11 10 00       	mov    $0x10110a,%eax
  10236f:	66 a3 20 02 11 00    	mov    %ax,0x110220
  102375:	c1 e8 10             	shr    $0x10,%eax
  102378:	66 a3 26 02 11 00    	mov    %ax,0x110226
  10237e:	b8 13 11 10 00       	mov    $0x101113,%eax
  102383:	66 a3 28 02 11 00    	mov    %ax,0x110228
  102389:	c1 e8 10             	shr    $0x10,%eax
  10238c:	66 a3 2e 02 11 00    	mov    %ax,0x11022e
  102392:	b8 1a 11 10 00       	mov    $0x10111a,%eax
  102397:	66 a3 30 02 11 00    	mov    %ax,0x110230
  10239d:	c1 e8 10             	shr    $0x10,%eax
  1023a0:	66 a3 36 02 11 00    	mov    %ax,0x110236
  1023a6:	b8 23 11 10 00       	mov    $0x101123,%eax
  1023ab:	66 a3 38 02 11 00    	mov    %ax,0x110238
  1023b1:	c1 e8 10             	shr    $0x10,%eax
  1023b4:	66 a3 3e 02 11 00    	mov    %ax,0x11023e
  1023ba:	b8 2c 11 10 00       	mov    $0x10112c,%eax
  1023bf:	66 a3 40 02 11 00    	mov    %ax,0x110240
  1023c5:	c1 e8 10             	shr    $0x10,%eax
  1023c8:	66 a3 46 02 11 00    	mov    %ax,0x110246
  1023ce:	b8 35 11 10 00       	mov    $0x101135,%eax
  1023d3:	66 a3 48 02 11 00    	mov    %ax,0x110248
  1023d9:	c1 e8 10             	shr    $0x10,%eax
  1023dc:	c7 05 fa 01 11 00 08 	movl   $0x8e000008,0x1101fa
  1023e3:	00 00 8e 
  1023e6:	c7 05 02 02 11 00 08 	movl   $0x8e000008,0x110202
  1023ed:	00 00 8e 
  1023f0:	c7 05 0a 02 11 00 08 	movl   $0x8e000008,0x11020a
  1023f7:	00 00 8e 
  1023fa:	c7 05 12 02 11 00 08 	movl   $0x8e000008,0x110212
  102401:	00 00 8e 
  102404:	c7 05 1a 02 11 00 08 	movl   $0x8e000008,0x11021a
  10240b:	00 00 8e 
  10240e:	c7 05 22 02 11 00 08 	movl   $0x8e000008,0x110222
  102415:	00 00 8e 
  102418:	c7 05 2a 02 11 00 08 	movl   $0x8e000008,0x11022a
  10241f:	00 00 8e 
  102422:	c7 05 32 02 11 00 08 	movl   $0x8e000008,0x110232
  102429:	00 00 8e 
  10242c:	c7 05 3a 02 11 00 08 	movl   $0x8e000008,0x11023a
  102433:	00 00 8e 
  102436:	c7 05 42 02 11 00 08 	movl   $0x8e000008,0x110242
  10243d:	00 00 8e 
  102440:	c7 05 4a 02 11 00 08 	movl   $0x8e000008,0x11024a
  102447:	00 00 8e 
  10244a:	66 a3 4e 02 11 00    	mov    %ax,0x11024e
  102450:	b8 3c 11 10 00       	mov    $0x10113c,%eax
  102455:	66 a3 50 02 11 00    	mov    %ax,0x110250
  10245b:	c1 e8 10             	shr    $0x10,%eax
  10245e:	66 a3 56 02 11 00    	mov    %ax,0x110256
  102464:	b8 45 11 10 00       	mov    $0x101145,%eax
  102469:	66 a3 58 02 11 00    	mov    %ax,0x110258
  10246f:	c1 e8 10             	shr    $0x10,%eax
  102472:	66 a3 5e 02 11 00    	mov    %ax,0x11025e
  102478:	b8 4e 11 10 00       	mov    $0x10114e,%eax
  10247d:	66 a3 60 02 11 00    	mov    %ax,0x110260
  102483:	c1 e8 10             	shr    $0x10,%eax
  102486:	66 a3 66 02 11 00    	mov    %ax,0x110266
  10248c:	b8 57 11 10 00       	mov    $0x101157,%eax
  102491:	66 a3 68 02 11 00    	mov    %ax,0x110268
  102497:	c1 e8 10             	shr    $0x10,%eax
  10249a:	66 a3 6e 02 11 00    	mov    %ax,0x11026e
  1024a0:	b8 60 11 10 00       	mov    $0x101160,%eax
  1024a5:	66 a3 70 02 11 00    	mov    %ax,0x110270
  1024ab:	c1 e8 10             	shr    $0x10,%eax
  1024ae:	66 a3 76 02 11 00    	mov    %ax,0x110276
  1024b4:	b8 66 11 10 00       	mov    $0x101166,%eax
  1024b9:	66 a3 78 02 11 00    	mov    %ax,0x110278
  1024bf:	c1 e8 10             	shr    $0x10,%eax
  1024c2:	66 a3 7e 02 11 00    	mov    %ax,0x11027e
  1024c8:	b8 6c 11 10 00       	mov    $0x10116c,%eax
  1024cd:	66 a3 80 02 11 00    	mov    %ax,0x110280
  1024d3:	c1 e8 10             	shr    $0x10,%eax
  1024d6:	66 a3 86 02 11 00    	mov    %ax,0x110286
  1024dc:	b8 72 11 10 00       	mov    $0x101172,%eax
  1024e1:	66 a3 88 02 11 00    	mov    %ax,0x110288
  1024e7:	c1 e8 10             	shr    $0x10,%eax
  1024ea:	66 a3 8e 02 11 00    	mov    %ax,0x11028e
  1024f0:	b8 78 11 10 00       	mov    $0x101178,%eax
  1024f5:	66 a3 90 02 11 00    	mov    %ax,0x110290
  1024fb:	c1 e8 10             	shr    $0x10,%eax
  1024fe:	66 a3 96 02 11 00    	mov    %ax,0x110296
  102504:	b8 7c 11 10 00       	mov    $0x10117c,%eax
  102509:	66 a3 98 02 11 00    	mov    %ax,0x110298
  10250f:	c1 e8 10             	shr    $0x10,%eax
  102512:	66 a3 9e 02 11 00    	mov    %ax,0x11029e
  102518:	b8 82 11 10 00       	mov    $0x101182,%eax
  10251d:	66 a3 a0 03 11 00    	mov    %ax,0x1103a0
  102523:	c1 e8 10             	shr    $0x10,%eax
  102526:	c7 05 52 02 11 00 08 	movl   $0x8e000008,0x110252
  10252d:	00 00 8e 
  102530:	c7 05 5a 02 11 00 08 	movl   $0x8e000008,0x11025a
  102537:	00 00 8e 
  10253a:	c7 05 62 02 11 00 08 	movl   $0x8e000008,0x110262
  102541:	00 00 8e 
  102544:	c7 05 6a 02 11 00 08 	movl   $0x8e000008,0x11026a
  10254b:	00 00 8e 
  10254e:	c7 05 72 02 11 00 08 	movl   $0x8e000008,0x110272
  102555:	00 00 8e 
  102558:	c7 05 7a 02 11 00 08 	movl   $0x8e000008,0x11027a
  10255f:	00 00 8e 
  102562:	c7 05 82 02 11 00 08 	movl   $0x8e000008,0x110282
  102569:	00 00 8e 
  10256c:	c7 05 8a 02 11 00 08 	movl   $0x8e000008,0x11028a
  102573:	00 00 8e 
  102576:	c7 05 92 02 11 00 08 	movl   $0x8e000008,0x110292
  10257d:	00 00 8e 
  102580:	c7 05 9a 02 11 00 08 	movl   $0x8e000008,0x11029a
  102587:	00 00 8e 
  10258a:	c7 05 a2 03 11 00 08 	movl   $0x8e000008,0x1103a2
  102591:	00 00 8e 
  102594:	66 a3 a6 03 11 00    	mov    %ax,0x1103a6
  10259a:	b8 88 11 10 00       	mov    $0x101188,%eax
  10259f:	66 a3 a8 03 11 00    	mov    %ax,0x1103a8
  1025a5:	c1 e8 10             	shr    $0x10,%eax
  1025a8:	66 a3 ae 03 11 00    	mov    %ax,0x1103ae
  1025ae:	b8 8e 11 10 00       	mov    $0x10118e,%eax
  1025b3:	66 a3 b0 03 11 00    	mov    %ax,0x1103b0
  1025b9:	c1 e8 10             	shr    $0x10,%eax
  1025bc:	66 a3 b6 03 11 00    	mov    %ax,0x1103b6
  1025c2:	b8 94 11 10 00       	mov    $0x101194,%eax
  1025c7:	66 a3 b8 03 11 00    	mov    %ax,0x1103b8
  1025cd:	c1 e8 10             	shr    $0x10,%eax
  1025d0:	66 a3 be 03 11 00    	mov    %ax,0x1103be
  1025d6:	b8 9a 11 10 00       	mov    $0x10119a,%eax
  1025db:	66 a3 c0 03 11 00    	mov    %ax,0x1103c0
  1025e1:	c1 e8 10             	shr    $0x10,%eax
  1025e4:	66 a3 c6 03 11 00    	mov    %ax,0x1103c6
  1025ea:	b8 a0 11 10 00       	mov    $0x1011a0,%eax
  1025ef:	66 a3 c8 03 11 00    	mov    %ax,0x1103c8
  1025f5:	c1 e8 10             	shr    $0x10,%eax
  1025f8:	66 a3 ce 03 11 00    	mov    %ax,0x1103ce
  1025fe:	b8 a6 11 10 00       	mov    $0x1011a6,%eax
  102603:	66 a3 d0 03 11 00    	mov    %ax,0x1103d0
  102609:	c1 e8 10             	shr    $0x10,%eax
  10260c:	66 a3 d6 03 11 00    	mov    %ax,0x1103d6
  102612:	b8 ac 11 10 00       	mov    $0x1011ac,%eax
  102617:	66 a3 d8 03 11 00    	mov    %ax,0x1103d8
  10261d:	c1 e8 10             	shr    $0x10,%eax
  102620:	66 a3 de 03 11 00    	mov    %ax,0x1103de
  102626:	b8 b2 11 10 00       	mov    $0x1011b2,%eax
  10262b:	66 a3 e0 03 11 00    	mov    %ax,0x1103e0
  102631:	c1 e8 10             	shr    $0x10,%eax
  102634:	66 a3 e6 03 11 00    	mov    %ax,0x1103e6
  10263a:	b8 b8 11 10 00       	mov    $0x1011b8,%eax
  10263f:	66 a3 e8 03 11 00    	mov    %ax,0x1103e8
  102645:	c1 e8 10             	shr    $0x10,%eax
  102648:	66 a3 ee 03 11 00    	mov    %ax,0x1103ee
  10264e:	b8 be 11 10 00       	mov    $0x1011be,%eax
  102653:	66 a3 f0 03 11 00    	mov    %ax,0x1103f0
  102659:	c1 e8 10             	shr    $0x10,%eax
  10265c:	66 a3 f6 03 11 00    	mov    %ax,0x1103f6
  102662:	b8 c4 11 10 00       	mov    $0x1011c4,%eax
  102667:	66 a3 f8 03 11 00    	mov    %ax,0x1103f8
  10266d:	c1 e8 10             	shr    $0x10,%eax
  102670:	c7 05 aa 03 11 00 08 	movl   $0x8e000008,0x1103aa
  102677:	00 00 8e 
  10267a:	c7 05 b2 03 11 00 08 	movl   $0x8e000008,0x1103b2
  102681:	00 00 8e 
  102684:	c7 05 ba 03 11 00 08 	movl   $0x8e000008,0x1103ba
  10268b:	00 00 8e 
  10268e:	c7 05 c2 03 11 00 08 	movl   $0x8e000008,0x1103c2
  102695:	00 00 8e 
  102698:	c7 05 ca 03 11 00 08 	movl   $0x8e000008,0x1103ca
  10269f:	00 00 8e 
  1026a2:	c7 05 d2 03 11 00 08 	movl   $0x8e000008,0x1103d2
  1026a9:	00 00 8e 
  1026ac:	c7 05 da 03 11 00 08 	movl   $0x8e000008,0x1103da
  1026b3:	00 00 8e 
  1026b6:	c7 05 e2 03 11 00 08 	movl   $0x8e000008,0x1103e2
  1026bd:	00 00 8e 
  1026c0:	c7 05 ea 03 11 00 08 	movl   $0x8e000008,0x1103ea
  1026c7:	00 00 8e 
  1026ca:	c7 05 f2 03 11 00 08 	movl   $0x8e000008,0x1103f2
  1026d1:	00 00 8e 
  1026d4:	c7 05 fa 03 11 00 08 	movl   $0x8e000008,0x1103fa
  1026db:	00 00 8e 
  1026de:	66 a3 fe 03 11 00    	mov    %ax,0x1103fe
  1026e4:	b8 ca 11 10 00       	mov    $0x1011ca,%eax
  1026e9:	66 a3 00 04 11 00    	mov    %ax,0x110400
  1026ef:	c1 e8 10             	shr    $0x10,%eax
  1026f2:	66 a3 06 04 11 00    	mov    %ax,0x110406
  1026f8:	b8 d0 11 10 00       	mov    $0x1011d0,%eax
  1026fd:	66 a3 08 04 11 00    	mov    %ax,0x110408
  102703:	c1 e8 10             	shr    $0x10,%eax
  102706:	66 a3 0e 04 11 00    	mov    %ax,0x11040e
  10270c:	b8 d6 11 10 00       	mov    $0x1011d6,%eax
  102711:	66 a3 10 04 11 00    	mov    %ax,0x110410
  102717:	c1 e8 10             	shr    $0x10,%eax
  10271a:	66 a3 16 04 11 00    	mov    %ax,0x110416
  102720:	b8 dc 11 10 00       	mov    $0x1011dc,%eax
  102725:	66 a3 18 04 11 00    	mov    %ax,0x110418
  10272b:	c1 e8 10             	shr    $0x10,%eax
  10272e:	66 a3 1e 04 11 00    	mov    %ax,0x11041e
  102734:	b8 ff 07 00 00       	mov    $0x7ff,%eax
  102739:	c7 04 24 80 01 11 00 	movl   $0x110180,(%esp)
  102740:	c7 05 02 04 11 00 08 	movl   $0x8e000008,0x110402
  102747:	00 00 8e 
  10274a:	c7 05 0a 04 11 00 08 	movl   $0x8e000008,0x11040a
  102751:	00 00 8e 
  102754:	c7 05 12 04 11 00 08 	movl   $0x8e000008,0x110412
  10275b:	00 00 8e 
  10275e:	c7 05 1a 04 11 00 08 	movl   $0x8e000008,0x11041a
  102765:	00 00 8e 
  102768:	66 a3 80 01 11 00    	mov    %ax,0x110180
  10276e:	c7 05 82 01 11 00 a0 	movl   $0x1101a0,0x110182
  102775:	01 11 00 
  102778:	e8 bb e8 ff ff       	call   101038 <idt_flush>
  10277d:	83 c4 14             	add    $0x14,%esp
  102780:	5b                   	pop    %ebx
  102781:	5e                   	pop    %esi
  102782:	c3                   	ret
  102783:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10278a:	00 
  10278b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00102790 <irq_register>:
  102790:	8b 54 24 04          	mov    0x4(%esp),%edx
  102794:	80 fa 0f             	cmp    $0xf,%dl
  102797:	76 07                	jbe    1027a0 <irq_register+0x10>
  102799:	c3                   	ret
  10279a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1027a0:	8b 44 24 08          	mov    0x8(%esp),%eax
  1027a4:	0f b6 ca             	movzbl %dl,%ecx
  1027a7:	89 04 8d 40 01 11 00 	mov    %eax,0x110140(,%ecx,4)
  1027ae:	80 fa 07             	cmp    $0x7,%dl
  1027b1:	77 0d                	ja     1027c0 <irq_register+0x30>
  1027b3:	e4 21                	in     $0x21,%al
  1027b5:	0f b3 c8             	btr    %ecx,%eax
  1027b8:	e6 21                	out    %al,$0x21
  1027ba:	c3                   	ret
  1027bb:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  1027c0:	e4 a1                	in     $0xa1,%al
  1027c2:	83 ea 08             	sub    $0x8,%edx
  1027c5:	0f b3 d0             	btr    %edx,%eax
  1027c8:	e6 a1                	out    %al,$0xa1
  1027ca:	c3                   	ret
  1027cb:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

001027d0 <irq_unregister>:
  1027d0:	53                   	push   %ebx
  1027d1:	8b 44 24 08          	mov    0x8(%esp),%eax
  1027d5:	3c 0f                	cmp    $0xf,%al
  1027d7:	77 22                	ja     1027fb <irq_unregister+0x2b>
  1027d9:	0f b6 c8             	movzbl %al,%ecx
  1027dc:	ba 21 00 00 00       	mov    $0x21,%edx
  1027e1:	c7 04 8d 40 01 11 00 	movl   $0x0,0x110140(,%ecx,4)
  1027e8:	00 00 00 00 
  1027ec:	3c 07                	cmp    $0x7,%al
  1027ee:	77 10                	ja     102800 <irq_unregister+0x30>
  1027f0:	ec                   	in     (%dx),%al
  1027f1:	bb 01 00 00 00       	mov    $0x1,%ebx
  1027f6:	d3 e3                	shl    %cl,%ebx
  1027f8:	09 d8                	or     %ebx,%eax
  1027fa:	ee                   	out    %al,(%dx)
  1027fb:	5b                   	pop    %ebx
  1027fc:	c3                   	ret
  1027fd:	8d 76 00             	lea    0x0(%esi),%esi
  102800:	83 e8 08             	sub    $0x8,%eax
  102803:	ba a1 00 00 00       	mov    $0xa1,%edx
  102808:	0f b6 c8             	movzbl %al,%ecx
  10280b:	eb e3                	jmp    1027f0 <irq_unregister+0x20>
  10280d:	8d 76 00             	lea    0x0(%esi),%esi

00102810 <exception_register>:
  102810:	8b 44 24 04          	mov    0x4(%esp),%eax
  102814:	3c 1f                	cmp    $0x1f,%al
  102816:	77 0e                	ja     102826 <exception_register+0x16>
  102818:	8b 54 24 08          	mov    0x8(%esp),%edx
  10281c:	0f b6 c0             	movzbl %al,%eax
  10281f:	89 14 85 c0 00 11 00 	mov    %edx,0x1100c0(,%eax,4)
  102826:	c3                   	ret
  102827:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10282e:	00 
  10282f:	90                   	nop

00102830 <irq_mask>:
  102830:	53                   	push   %ebx
  102831:	8b 44 24 08          	mov    0x8(%esp),%eax
  102835:	3c 07                	cmp    $0x7,%al
  102837:	76 17                	jbe    102850 <irq_mask+0x20>
  102839:	8d 48 f8             	lea    -0x8(%eax),%ecx
  10283c:	ba a1 00 00 00       	mov    $0xa1,%edx
  102841:	ec                   	in     (%dx),%al
  102842:	bb 01 00 00 00       	mov    $0x1,%ebx
  102847:	d3 e3                	shl    %cl,%ebx
  102849:	09 d8                	or     %ebx,%eax
  10284b:	ee                   	out    %al,(%dx)
  10284c:	5b                   	pop    %ebx
  10284d:	c3                   	ret
  10284e:	66 90                	xchg   %ax,%ax
  102850:	89 c1                	mov    %eax,%ecx
  102852:	ba 21 00 00 00       	mov    $0x21,%edx
  102857:	eb e8                	jmp    102841 <irq_mask+0x11>
  102859:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00102860 <irq_unmask>:
  102860:	8b 54 24 04          	mov    0x4(%esp),%edx
  102864:	80 fa 07             	cmp    $0x7,%dl
  102867:	77 0f                	ja     102878 <irq_unmask+0x18>
  102869:	e4 21                	in     $0x21,%al
  10286b:	0f b3 d0             	btr    %edx,%eax
  10286e:	e6 21                	out    %al,$0x21
  102870:	c3                   	ret
  102871:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  102878:	83 ea 08             	sub    $0x8,%edx
  10287b:	e4 a1                	in     $0xa1,%al
  10287d:	0f b3 d0             	btr    %edx,%eax
  102880:	e6 a1                	out    %al,$0xa1
  102882:	80 fa 07             	cmp    $0x7,%dl
  102885:	76 07                	jbe    10288e <irq_unmask+0x2e>
  102887:	e4 21                	in     $0x21,%al
  102889:	83 e0 fb             	and    $0xfffffffb,%eax
  10288c:	e6 21                	out    %al,$0x21
  10288e:	c3                   	ret
  10288f:	90                   	nop

00102890 <pic_send_eoi>:
  102890:	80 7c 24 04 07       	cmpb   $0x7,0x4(%esp)
  102895:	76 07                	jbe    10289e <pic_send_eoi+0xe>
  102897:	b8 20 00 00 00       	mov    $0x20,%eax
  10289c:	e6 a0                	out    %al,$0xa0
  10289e:	b8 20 00 00 00       	mov    $0x20,%eax
  1028a3:	e6 20                	out    %al,$0x20
  1028a5:	c3                   	ret
  1028a6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1028ad:	00 
  1028ae:	66 90                	xchg   %ax,%ax

001028b0 <isr_dispatch>:
  1028b0:	53                   	push   %ebx
  1028b1:	83 ec 08             	sub    $0x8,%esp
  1028b4:	8b 54 24 10          	mov    0x10(%esp),%edx
  1028b8:	8b 42 30             	mov    0x30(%edx),%eax
  1028bb:	83 f8 1f             	cmp    $0x1f,%eax
  1028be:	77 18                	ja     1028d8 <isr_dispatch+0x28>
  1028c0:	8b 0c 85 c0 00 11 00 	mov    0x1100c0(,%eax,4),%ecx
  1028c7:	85 c9                	test   %ecx,%ecx
  1028c9:	74 44                	je     10290f <isr_dispatch+0x5f>
  1028cb:	89 54 24 10          	mov    %edx,0x10(%esp)
  1028cf:	83 c4 08             	add    $0x8,%esp
  1028d2:	5b                   	pop    %ebx
  1028d3:	ff e1                	jmp    *%ecx
  1028d5:	8d 76 00             	lea    0x0(%esi),%esi
  1028d8:	83 f8 2f             	cmp    $0x2f,%eax
  1028db:	77 2d                	ja     10290a <isr_dispatch+0x5a>
  1028dd:	8d 58 e0             	lea    -0x20(%eax),%ebx
  1028e0:	0f b6 c3             	movzbl %bl,%eax
  1028e3:	8b 04 85 40 01 11 00 	mov    0x110140(,%eax,4),%eax
  1028ea:	85 c0                	test   %eax,%eax
  1028ec:	74 09                	je     1028f7 <isr_dispatch+0x47>
  1028ee:	83 ec 0c             	sub    $0xc,%esp
  1028f1:	52                   	push   %edx
  1028f2:	ff d0                	call   *%eax
  1028f4:	83 c4 10             	add    $0x10,%esp
  1028f7:	80 fb 07             	cmp    $0x7,%bl
  1028fa:	76 07                	jbe    102903 <isr_dispatch+0x53>
  1028fc:	b8 20 00 00 00       	mov    $0x20,%eax
  102901:	e6 a0                	out    %al,$0xa0
  102903:	b8 20 00 00 00       	mov    $0x20,%eax
  102908:	e6 20                	out    %al,$0x20
  10290a:	83 c4 08             	add    $0x8,%esp
  10290d:	5b                   	pop    %ebx
  10290e:	c3                   	ret
  10290f:	51                   	push   %ecx
  102910:	51                   	push   %ecx
  102911:	ff 72 1c             	push   0x1c(%edx)
  102914:	ff 72 18             	push   0x18(%edx)
  102917:	ff 72 10             	push   0x10(%edx)
  10291a:	ff 72 14             	push   0x14(%edx)
  10291d:	ff 72 24             	push   0x24(%edx)
  102920:	ff 72 28             	push   0x28(%edx)
  102923:	ff 72 20             	push   0x20(%edx)
  102926:	ff 72 2c             	push   0x2c(%edx)
  102929:	ff 72 3c             	push   0x3c(%edx)
  10292c:	ff 72 38             	push   0x38(%edx)
  10292f:	ff 72 34             	push   0x34(%edx)
  102932:	ff 34 85 e0 a5 10 00 	push   0x10a5e0(,%eax,4)
  102939:	50                   	push   %eax
  10293a:	68 b4 98 10 00       	push   $0x1098b4
  10293f:	e8 cc 00 00 00       	call   102a10 <kpanic>
  102944:	66 90                	xchg   %ax,%ax
  102946:	66 90                	xchg   %ax,%ax
  102948:	66 90                	xchg   %ax,%ax
  10294a:	66 90                	xchg   %ax,%ax
  10294c:	66 90                	xchg   %ax,%ax
  10294e:	66 90                	xchg   %ax,%ax

00102950 <cpu_init>:
  102950:	83 ec 10             	sub    $0x10,%esp
  102953:	6a 58                	push   $0x58
  102955:	6a 00                	push   $0x0
  102957:	68 a0 09 11 00       	push   $0x1109a0
  10295c:	e8 ff eb ff ff       	call   101560 <memset>
  102961:	9c                   	pushf
  102962:	58                   	pop    %eax
  102963:	89 c2                	mov    %eax,%edx
  102965:	35 00 00 20 00       	xor    $0x200000,%eax
  10296a:	50                   	push   %eax
  10296b:	9d                   	popf
  10296c:	9c                   	pushf
  10296d:	58                   	pop    %eax
  10296e:	83 c4 0c             	add    $0xc,%esp
  102971:	c6 05 f4 09 11 00 00 	movb   $0x0,0x1109f4
  102978:	6a 08                	push   $0x8
  10297a:	68 c9 92 10 00       	push   $0x1092c9
  10297f:	68 a0 09 11 00       	push   $0x1109a0
  102984:	e8 27 ec ff ff       	call   1015b0 <memcpy>
  102989:	83 c4 1c             	add    $0x1c,%esp
  10298c:	c3                   	ret
  10298d:	8d 76 00             	lea    0x0(%esi),%esi

00102990 <cpu_get_info>:
  102990:	b8 a0 09 11 00       	mov    $0x1109a0,%eax
  102995:	c3                   	ret
  102996:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10299d:	00 
  10299e:	66 90                	xchg   %ax,%ax

001029a0 <cpu_dump>:
  1029a0:	83 ec 10             	sub    $0x10,%esp
  1029a3:	68 a0 09 11 00       	push   $0x1109a0
  1029a8:	68 d1 92 10 00       	push   $0x1092d1
  1029ad:	6a 01                	push   $0x1
  1029af:	e8 7c 21 00 00       	call   104b30 <klog>
  1029b4:	83 c4 0c             	add    $0xc,%esp
  1029b7:	68 ad 09 11 00       	push   $0x1109ad
  1029bc:	68 e3 92 10 00       	push   $0x1092e3
  1029c1:	6a 01                	push   $0x1
  1029c3:	e8 68 21 00 00       	call   104b30 <klog>
  1029c8:	58                   	pop    %eax
  1029c9:	ff 35 e8 09 11 00    	push   0x1109e8
  1029cf:	ff 35 e4 09 11 00    	push   0x1109e4
  1029d5:	ff 35 e0 09 11 00    	push   0x1109e0
  1029db:	68 64 99 10 00       	push   $0x109964
  1029e0:	6a 01                	push   $0x1
  1029e2:	e8 49 21 00 00       	call   104b30 <klog>
  1029e7:	83 c4 20             	add    $0x20,%esp
  1029ea:	ff 35 f0 09 11 00    	push   0x1109f0
  1029f0:	ff 35 ec 09 11 00    	push   0x1109ec
  1029f6:	68 90 99 10 00       	push   $0x109990
  1029fb:	6a 01                	push   $0x1
  1029fd:	e8 2e 21 00 00       	call   104b30 <klog>
  102a02:	83 c4 1c             	add    $0x1c,%esp
  102a05:	c3                   	ret
  102a06:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  102a0d:	00 
  102a0e:	66 90                	xchg   %ax,%ax

00102a10 <kpanic>:
  102a10:	53                   	push   %ebx
  102a11:	81 ec 10 02 00 00    	sub    $0x210,%esp
  102a17:	fa                   	cli
  102a18:	6a 04                	push   $0x4
  102a1a:	6a 0f                	push   $0xf
  102a1c:	e8 9f 19 00 00       	call   1043c0 <vga_set_color>
  102a21:	c7 04 24 f5 92 10 00 	movl   $0x1092f5,(%esp)
  102a28:	e8 e3 1b 00 00       	call   104610 <vga_puts>
  102a2d:	8d 84 24 24 02 00 00 	lea    0x224(%esp),%eax
  102a34:	50                   	push   %eax
  102a35:	ff b4 24 24 02 00 00 	push   0x224(%esp)
  102a3c:	68 00 02 00 00       	push   $0x200
  102a41:	8d 5c 24 1c          	lea    0x1c(%esp),%ebx
  102a45:	53                   	push   %ebx
  102a46:	e8 55 ef ff ff       	call   1019a0 <vsnprintf>
  102a4b:	83 c4 14             	add    $0x14,%esp
  102a4e:	53                   	push   %ebx
  102a4f:	e8 bc 1b 00 00       	call   104610 <vga_puts>
  102a54:	c7 04 24 11 93 10 00 	movl   $0x109311,(%esp)
  102a5b:	e8 90 1f 00 00       	call   1049f0 <serial_puts>
  102a60:	89 1c 24             	mov    %ebx,(%esp)
  102a63:	e8 88 1f 00 00       	call   1049f0 <serial_puts>
  102a68:	c7 04 24 bf 95 10 00 	movl   $0x1095bf,(%esp)
  102a6f:	e8 7c 1f 00 00       	call   1049f0 <serial_puts>
  102a74:	89 dc                	mov    %ebx,%esp
  102a76:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  102a7d:	00 
  102a7e:	66 90                	xchg   %ax,%ax
  102a80:	f4                   	hlt
  102a81:	eb fd                	jmp    102a80 <kpanic+0x70>
  102a83:	66 90                	xchg   %ax,%ax
  102a85:	66 90                	xchg   %ax,%ax
  102a87:	66 90                	xchg   %ax,%ax
  102a89:	66 90                	xchg   %ax,%ax
  102a8b:	66 90                	xchg   %ax,%ax
  102a8d:	66 90                	xchg   %ax,%ax
  102a8f:	90                   	nop

00102a90 <pmm_mark_used>:
  102a90:	55                   	push   %ebp
  102a91:	57                   	push   %edi
  102a92:	56                   	push   %esi
  102a93:	53                   	push   %ebx
  102a94:	83 ec 08             	sub    $0x8,%esp
  102a97:	8b 44 24 1c          	mov    0x1c(%esp),%eax
  102a9b:	8b 74 24 20          	mov    0x20(%esp),%esi
  102a9f:	c1 e8 0c             	shr    $0xc,%eax
  102aa2:	c1 ee 0c             	shr    $0xc,%esi
  102aa5:	89 34 24             	mov    %esi,(%esp)
  102aa8:	39 f0                	cmp    %esi,%eax
  102aaa:	73 5b                	jae    102b07 <pmm_mark_used+0x77>
  102aac:	c6 44 24 07 00       	movb   $0x0,0x7(%esp)
  102ab1:	8b 3d fc 09 11 00    	mov    0x1109fc,%edi
  102ab7:	8b 2d 00 0a 11 00    	mov    0x110a00,%ebp
  102abd:	8b 35 f8 09 11 00    	mov    0x1109f8,%esi
  102ac3:	eb 31                	jmp    102af6 <pmm_mark_used+0x66>
  102ac5:	8d 76 00             	lea    0x0(%esi),%esi
  102ac8:	89 c2                	mov    %eax,%edx
  102aca:	89 c3                	mov    %eax,%ebx
  102acc:	c1 ea 05             	shr    $0x5,%edx
  102acf:	83 e3 1f             	and    $0x1f,%ebx
  102ad2:	8d 4c 95 00          	lea    0x0(%ebp,%edx,4),%ecx
  102ad6:	8b 11                	mov    (%ecx),%edx
  102ad8:	0f a3 c2             	bt     %eax,%edx
  102adb:	72 11                	jb     102aee <pmm_mark_used+0x5e>
  102add:	0f ab da             	bts    %ebx,%edx
  102ae0:	89 11                	mov    %edx,(%ecx)
  102ae2:	85 f6                	test   %esi,%esi
  102ae4:	74 08                	je     102aee <pmm_mark_used+0x5e>
  102ae6:	c6 44 24 07 01       	movb   $0x1,0x7(%esp)
  102aeb:	83 ee 01             	sub    $0x1,%esi
  102aee:	83 c0 01             	add    $0x1,%eax
  102af1:	39 04 24             	cmp    %eax,(%esp)
  102af4:	74 04                	je     102afa <pmm_mark_used+0x6a>
  102af6:	39 f8                	cmp    %edi,%eax
  102af8:	72 ce                	jb     102ac8 <pmm_mark_used+0x38>
  102afa:	80 7c 24 07 00       	cmpb   $0x0,0x7(%esp)
  102aff:	74 06                	je     102b07 <pmm_mark_used+0x77>
  102b01:	89 35 f8 09 11 00    	mov    %esi,0x1109f8
  102b07:	83 c4 08             	add    $0x8,%esp
  102b0a:	5b                   	pop    %ebx
  102b0b:	5e                   	pop    %esi
  102b0c:	5f                   	pop    %edi
  102b0d:	5d                   	pop    %ebp
  102b0e:	c3                   	ret
  102b0f:	90                   	nop

00102b10 <pmm_mark_free>:
  102b10:	55                   	push   %ebp
  102b11:	57                   	push   %edi
  102b12:	56                   	push   %esi
  102b13:	53                   	push   %ebx
  102b14:	83 ec 0c             	sub    $0xc,%esp
  102b17:	8b 44 24 20          	mov    0x20(%esp),%eax
  102b1b:	8b 6c 24 24          	mov    0x24(%esp),%ebp
  102b1f:	05 ff 0f 00 00       	add    $0xfff,%eax
  102b24:	c1 ed 0c             	shr    $0xc,%ebp
  102b27:	c1 e8 0c             	shr    $0xc,%eax
  102b2a:	39 e8                	cmp    %ebp,%eax
  102b2c:	73 6d                	jae    102b9b <pmm_mark_free+0x8b>
  102b2e:	8b 3d 00 0a 11 00    	mov    0x110a00,%edi
  102b34:	8b 1d f8 09 11 00    	mov    0x1109f8,%ebx
  102b3a:	c6 44 24 07 00       	movb   $0x0,0x7(%esp)
  102b3f:	8b 35 fc 09 11 00    	mov    0x1109fc,%esi
  102b45:	89 5c 24 08          	mov    %ebx,0x8(%esp)
  102b49:	89 3c 24             	mov    %edi,(%esp)
  102b4c:	eb 39                	jmp    102b87 <pmm_mark_free+0x77>
  102b4e:	66 90                	xchg   %ax,%ax
  102b50:	8b 3c 24             	mov    (%esp),%edi
  102b53:	89 c2                	mov    %eax,%edx
  102b55:	89 c1                	mov    %eax,%ecx
  102b57:	c1 ea 05             	shr    $0x5,%edx
  102b5a:	83 e1 1f             	and    $0x1f,%ecx
  102b5d:	8d 14 97             	lea    (%edi,%edx,4),%edx
  102b60:	8b 1a                	mov    (%edx),%ebx
  102b62:	0f a3 c3             	bt     %eax,%ebx
  102b65:	73 19                	jae    102b80 <pmm_mark_free+0x70>
  102b67:	bf 01 00 00 00       	mov    $0x1,%edi
  102b6c:	83 44 24 08 01       	addl   $0x1,0x8(%esp)
  102b71:	d3 e7                	shl    %cl,%edi
  102b73:	c6 44 24 07 01       	movb   $0x1,0x7(%esp)
  102b78:	89 f9                	mov    %edi,%ecx
  102b7a:	f7 d1                	not    %ecx
  102b7c:	21 d9                	and    %ebx,%ecx
  102b7e:	89 0a                	mov    %ecx,(%edx)
  102b80:	83 c0 01             	add    $0x1,%eax
  102b83:	39 c5                	cmp    %eax,%ebp
  102b85:	74 04                	je     102b8b <pmm_mark_free+0x7b>
  102b87:	39 f0                	cmp    %esi,%eax
  102b89:	72 c5                	jb     102b50 <pmm_mark_free+0x40>
  102b8b:	80 7c 24 07 00       	cmpb   $0x0,0x7(%esp)
  102b90:	74 09                	je     102b9b <pmm_mark_free+0x8b>
  102b92:	8b 44 24 08          	mov    0x8(%esp),%eax
  102b96:	a3 f8 09 11 00       	mov    %eax,0x1109f8
  102b9b:	83 c4 0c             	add    $0xc,%esp
  102b9e:	5b                   	pop    %ebx
  102b9f:	5e                   	pop    %esi
  102ba0:	5f                   	pop    %edi
  102ba1:	5d                   	pop    %ebp
  102ba2:	c3                   	ret
  102ba3:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  102baa:	00 
  102bab:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00102bb0 <pmm_init>:
  102bb0:	55                   	push   %ebp
  102bb1:	57                   	push   %edi
  102bb2:	56                   	push   %esi
  102bb3:	53                   	push   %ebx
  102bb4:	83 ec 1c             	sub    $0x1c,%esp
  102bb7:	8b 5c 24 30          	mov    0x30(%esp),%ebx
  102bbb:	f6 03 40             	testb  $0x40,(%ebx)
  102bbe:	74 66                	je     102c26 <pmm_init+0x76>
  102bc0:	8b 43 2c             	mov    0x2c(%ebx),%eax
  102bc3:	89 44 24 08          	mov    %eax,0x8(%esp)
  102bc7:	85 c0                	test   %eax,%eax
  102bc9:	74 5b                	je     102c26 <pmm_init+0x76>
  102bcb:	8b 43 30             	mov    0x30(%ebx),%eax
  102bce:	31 d2                	xor    %edx,%edx
  102bd0:	31 ed                	xor    %ebp,%ebp
  102bd2:	89 44 24 0c          	mov    %eax,0xc(%esp)
  102bd6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  102bdd:	00 
  102bde:	66 90                	xchg   %ax,%ax
  102be0:	8b 44 24 0c          	mov    0xc(%esp),%eax
  102be4:	b9 00 00 00 00       	mov    $0x0,%ecx
  102be9:	01 d0                	add    %edx,%eax
  102beb:	8b 70 04             	mov    0x4(%eax),%esi
  102bee:	03 70 0c             	add    0xc(%eax),%esi
  102bf1:	8b 78 08             	mov    0x8(%eax),%edi
  102bf4:	13 78 10             	adc    0x10(%eax),%edi
  102bf7:	39 f5                	cmp    %esi,%ebp
  102bf9:	19 f9                	sbb    %edi,%ecx
  102bfb:	73 13                	jae    102c10 <pmm_init+0x60>
  102bfd:	83 78 14 01          	cmpl   $0x1,0x14(%eax)
  102c01:	75 0d                	jne    102c10 <pmm_init+0x60>
  102c03:	85 ff                	test   %edi,%edi
  102c05:	bd ff ff ff ff       	mov    $0xffffffff,%ebp
  102c0a:	0f 44 ee             	cmove  %esi,%ebp
  102c0d:	8d 76 00             	lea    0x0(%esi),%esi
  102c10:	8b 00                	mov    (%eax),%eax
  102c12:	8d 54 02 04          	lea    0x4(%edx,%eax,1),%edx
  102c16:	8b 44 24 08          	mov    0x8(%esp),%eax
  102c1a:	39 c2                	cmp    %eax,%edx
  102c1c:	72 c2                	jb     102be0 <pmm_init+0x30>
  102c1e:	8b 5c 24 30          	mov    0x30(%esp),%ebx
  102c22:	85 ed                	test   %ebp,%ebp
  102c24:	75 0c                	jne    102c32 <pmm_init+0x82>
  102c26:	8b 43 08             	mov    0x8(%ebx),%eax
  102c29:	8d a8 00 04 00 00    	lea    0x400(%eax),%ebp
  102c2f:	c1 e5 0a             	shl    $0xa,%ebp
  102c32:	c7 05 f8 09 11 00 00 	movl   $0x0,0x1109f8
  102c39:	00 00 00 
  102c3c:	b8 00 00 00 40       	mov    $0x40000000,%eax
  102c41:	c7 05 00 0a 11 00 00 	movl   $0x124000,0x110a00
  102c48:	40 12 00 
  102c4b:	39 c5                	cmp    %eax,%ebp
  102c4d:	0f 46 c5             	cmovbe %ebp,%eax
  102c50:	83 ec 04             	sub    $0x4,%esp
  102c53:	89 c7                	mov    %eax,%edi
  102c55:	8d 80 ff 0f 00 00    	lea    0xfff(%eax),%eax
  102c5b:	c1 e8 0c             	shr    $0xc,%eax
  102c5e:	a3 fc 09 11 00       	mov    %eax,0x1109fc
  102c63:	83 c0 07             	add    $0x7,%eax
  102c66:	c1 e8 03             	shr    $0x3,%eax
  102c69:	05 ff 0f 00 00       	add    $0xfff,%eax
  102c6e:	25 00 f0 ff ff       	and    $0xfffff000,%eax
  102c73:	50                   	push   %eax
  102c74:	89 c6                	mov    %eax,%esi
  102c76:	68 ff 00 00 00       	push   $0xff
  102c7b:	68 00 40 12 00       	push   $0x124000
  102c80:	e8 db e8 ff ff       	call   101560 <memset>
  102c85:	83 c4 10             	add    $0x10,%esp
  102c88:	f6 03 40             	testb  $0x40,(%ebx)
  102c8b:	0f 84 77 01 00 00    	je     102e08 <pmm_init+0x258>
  102c91:	8b 43 2c             	mov    0x2c(%ebx),%eax
  102c94:	31 ed                	xor    %ebp,%ebp
  102c96:	85 c0                	test   %eax,%eax
  102c98:	0f 84 8a 00 00 00    	je     102d28 <pmm_init+0x178>
  102c9e:	89 7c 24 08          	mov    %edi,0x8(%esp)
  102ca2:	89 74 24 0c          	mov    %esi,0xc(%esp)
  102ca6:	89 5c 24 30          	mov    %ebx,0x30(%esp)
  102caa:	eb 13                	jmp    102cbf <pmm_init+0x10f>
  102cac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  102cb0:	8b 03                	mov    (%ebx),%eax
  102cb2:	8d 6c 05 04          	lea    0x4(%ebp,%eax,1),%ebp
  102cb6:	8b 44 24 30          	mov    0x30(%esp),%eax
  102cba:	3b 68 2c             	cmp    0x2c(%eax),%ebp
  102cbd:	73 61                	jae    102d20 <pmm_init+0x170>
  102cbf:	8b 44 24 30          	mov    0x30(%esp),%eax
  102cc3:	89 eb                	mov    %ebp,%ebx
  102cc5:	03 58 30             	add    0x30(%eax),%ebx
  102cc8:	83 7b 14 01          	cmpl   $0x1,0x14(%ebx)
  102ccc:	75 e2                	jne    102cb0 <pmm_init+0x100>
  102cce:	8b 43 04             	mov    0x4(%ebx),%eax
  102cd1:	8b 53 08             	mov    0x8(%ebx),%edx
  102cd4:	89 c6                	mov    %eax,%esi
  102cd6:	89 d7                	mov    %edx,%edi
  102cd8:	03 73 0c             	add    0xc(%ebx),%esi
  102cdb:	13 7b 10             	adc    0x10(%ebx),%edi
  102cde:	83 fa 00             	cmp    $0x0,%edx
  102ce1:	0f 86 d9 00 00 00    	jbe    102dc0 <pmm_init+0x210>
  102ce7:	83 ff 00             	cmp    $0x0,%edi
  102cea:	0f 87 f8 00 00 00    	ja     102de8 <pmm_init+0x238>
  102cf0:	89 f2                	mov    %esi,%edx
  102cf2:	31 c0                	xor    %eax,%eax
  102cf4:	81 e2 00 f0 ff ff    	and    $0xfffff000,%edx
  102cfa:	39 d0                	cmp    %edx,%eax
  102cfc:	73 b2                	jae    102cb0 <pmm_init+0x100>
  102cfe:	83 ec 08             	sub    $0x8,%esp
  102d01:	52                   	push   %edx
  102d02:	50                   	push   %eax
  102d03:	e8 08 fe ff ff       	call   102b10 <pmm_mark_free>
  102d08:	83 c4 10             	add    $0x10,%esp
  102d0b:	8b 03                	mov    (%ebx),%eax
  102d0d:	8d 6c 05 04          	lea    0x4(%ebp,%eax,1),%ebp
  102d11:	8b 44 24 30          	mov    0x30(%esp),%eax
  102d15:	3b 68 2c             	cmp    0x2c(%eax),%ebp
  102d18:	72 a5                	jb     102cbf <pmm_init+0x10f>
  102d1a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  102d20:	8b 7c 24 08          	mov    0x8(%esp),%edi
  102d24:	8b 74 24 0c          	mov    0xc(%esp),%esi
  102d28:	8b 1d fc 09 11 00    	mov    0x1109fc,%ebx
  102d2e:	85 db                	test   %ebx,%ebx
  102d30:	74 0c                	je     102d3e <pmm_init+0x18e>
  102d32:	8b 15 00 0a 11 00    	mov    0x110a00,%edx
  102d38:	8b 02                	mov    (%edx),%eax
  102d3a:	a8 01                	test   $0x1,%al
  102d3c:	74 62                	je     102da0 <pmm_init+0x1f0>
  102d3e:	b8 ff 4f 12 00       	mov    $0x124fff,%eax
  102d43:	83 ec 08             	sub    $0x8,%esp
  102d46:	c1 ef 14             	shr    $0x14,%edi
  102d49:	25 00 f0 ff ff       	and    $0xfffff000,%eax
  102d4e:	50                   	push   %eax
  102d4f:	b8 00 00 10 00       	mov    $0x100000,%eax
  102d54:	25 00 f0 ff ff       	and    $0xfffff000,%eax
  102d59:	50                   	push   %eax
  102d5a:	e8 31 fd ff ff       	call   102a90 <pmm_mark_used>
  102d5f:	58                   	pop    %eax
  102d60:	8d 86 ff 4f 12 00    	lea    0x124fff(%esi),%eax
  102d66:	5a                   	pop    %edx
  102d67:	25 00 f0 ff ff       	and    $0xfffff000,%eax
  102d6c:	c1 ee 0a             	shr    $0xa,%esi
  102d6f:	50                   	push   %eax
  102d70:	b8 00 40 12 00       	mov    $0x124000,%eax
  102d75:	25 00 f0 ff ff       	and    $0xfffff000,%eax
  102d7a:	50                   	push   %eax
  102d7b:	e8 10 fd ff ff       	call   102a90 <pmm_mark_used>
  102d80:	59                   	pop    %ecx
  102d81:	ff 35 f8 09 11 00    	push   0x1109f8
  102d87:	56                   	push   %esi
  102d88:	57                   	push   %edi
  102d89:	68 b8 99 10 00       	push   $0x1099b8
  102d8e:	6a 01                	push   $0x1
  102d90:	e8 9b 1d 00 00       	call   104b30 <klog>
  102d95:	83 c4 3c             	add    $0x3c,%esp
  102d98:	5b                   	pop    %ebx
  102d99:	5e                   	pop    %esi
  102d9a:	5f                   	pop    %edi
  102d9b:	5d                   	pop    %ebp
  102d9c:	c3                   	ret
  102d9d:	8d 76 00             	lea    0x0(%esi),%esi
  102da0:	83 c8 01             	or     $0x1,%eax
  102da3:	89 02                	mov    %eax,(%edx)
  102da5:	a1 f8 09 11 00       	mov    0x1109f8,%eax
  102daa:	85 c0                	test   %eax,%eax
  102dac:	74 90                	je     102d3e <pmm_init+0x18e>
  102dae:	83 e8 01             	sub    $0x1,%eax
  102db1:	a3 f8 09 11 00       	mov    %eax,0x1109f8
  102db6:	eb 86                	jmp    102d3e <pmm_init+0x18e>
  102db8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  102dbf:	00 
  102dc0:	05 ff 0f 00 00       	add    $0xfff,%eax
  102dc5:	ba 00 f0 ff ff       	mov    $0xfffff000,%edx
  102dca:	25 00 f0 ff ff       	and    $0xfffff000,%eax
  102dcf:	83 ff 00             	cmp    $0x0,%edi
  102dd2:	0f 87 22 ff ff ff    	ja     102cfa <pmm_init+0x14a>
  102dd8:	89 f2                	mov    %esi,%edx
  102dda:	81 e2 00 f0 ff ff    	and    $0xfffff000,%edx
  102de0:	e9 15 ff ff ff       	jmp    102cfa <pmm_init+0x14a>
  102de5:	8d 76 00             	lea    0x0(%esi),%esi
  102de8:	83 ec 08             	sub    $0x8,%esp
  102deb:	ba 00 f0 ff ff       	mov    $0xfffff000,%edx
  102df0:	31 c0                	xor    %eax,%eax
  102df2:	52                   	push   %edx
  102df3:	50                   	push   %eax
  102df4:	e8 17 fd ff ff       	call   102b10 <pmm_mark_free>
  102df9:	83 c4 10             	add    $0x10,%esp
  102dfc:	e9 0a ff ff ff       	jmp    102d0b <pmm_init+0x15b>
  102e01:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  102e08:	8b 43 04             	mov    0x4(%ebx),%eax
  102e0b:	83 ec 08             	sub    $0x8,%esp
  102e0e:	c1 e0 0a             	shl    $0xa,%eax
  102e11:	50                   	push   %eax
  102e12:	68 00 10 00 00       	push   $0x1000
  102e17:	e8 f4 fc ff ff       	call   102b10 <pmm_mark_free>
  102e1c:	5d                   	pop    %ebp
  102e1d:	58                   	pop    %eax
  102e1e:	57                   	push   %edi
  102e1f:	68 00 00 10 00       	push   $0x100000
  102e24:	e8 e7 fc ff ff       	call   102b10 <pmm_mark_free>
  102e29:	83 c4 10             	add    $0x10,%esp
  102e2c:	e9 f7 fe ff ff       	jmp    102d28 <pmm_init+0x178>
  102e31:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  102e38:	00 
  102e39:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00102e40 <pmm_alloc_page>:
  102e40:	57                   	push   %edi
  102e41:	56                   	push   %esi
  102e42:	53                   	push   %ebx
  102e43:	8b 1d fc 09 11 00    	mov    0x1109fc,%ebx
  102e49:	83 fb 01             	cmp    $0x1,%ebx
  102e4c:	76 35                	jbe    102e83 <pmm_alloc_page+0x43>
  102e4e:	8b 3d 00 0a 11 00    	mov    0x110a00,%edi
  102e54:	8b 17                	mov    (%edi),%edx
  102e56:	f6 c2 02             	test   $0x2,%dl
  102e59:	74 67                	je     102ec2 <pmm_alloc_page+0x82>
  102e5b:	b8 01 00 00 00       	mov    $0x1,%eax
  102e60:	eb 1a                	jmp    102e7c <pmm_alloc_page+0x3c>
  102e62:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  102e68:	89 c2                	mov    %eax,%edx
  102e6a:	89 c1                	mov    %eax,%ecx
  102e6c:	c1 ea 05             	shr    $0x5,%edx
  102e6f:	83 e1 1f             	and    $0x1f,%ecx
  102e72:	8d 34 97             	lea    (%edi,%edx,4),%esi
  102e75:	8b 16                	mov    (%esi),%edx
  102e77:	0f a3 c2             	bt     %eax,%edx
  102e7a:	73 14                	jae    102e90 <pmm_alloc_page+0x50>
  102e7c:	83 c0 01             	add    $0x1,%eax
  102e7f:	39 d8                	cmp    %ebx,%eax
  102e81:	75 e5                	jne    102e68 <pmm_alloc_page+0x28>
  102e83:	31 db                	xor    %ebx,%ebx
  102e85:	89 d8                	mov    %ebx,%eax
  102e87:	5b                   	pop    %ebx
  102e88:	5e                   	pop    %esi
  102e89:	5f                   	pop    %edi
  102e8a:	c3                   	ret
  102e8b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  102e90:	c1 e0 0c             	shl    $0xc,%eax
  102e93:	89 c3                	mov    %eax,%ebx
  102e95:	89 c7                	mov    %eax,%edi
  102e97:	b8 01 00 00 00       	mov    $0x1,%eax
  102e9c:	d3 e0                	shl    %cl,%eax
  102e9e:	83 ec 04             	sub    $0x4,%esp
  102ea1:	09 d0                	or     %edx,%eax
  102ea3:	83 2d f8 09 11 00 01 	subl   $0x1,0x1109f8
  102eaa:	89 06                	mov    %eax,(%esi)
  102eac:	68 00 10 00 00       	push   $0x1000
  102eb1:	6a 00                	push   $0x0
  102eb3:	57                   	push   %edi
  102eb4:	e8 a7 e6 ff ff       	call   101560 <memset>
  102eb9:	83 c4 10             	add    $0x10,%esp
  102ebc:	89 d8                	mov    %ebx,%eax
  102ebe:	5b                   	pop    %ebx
  102ebf:	5e                   	pop    %esi
  102ec0:	5f                   	pop    %edi
  102ec1:	c3                   	ret
  102ec2:	89 fe                	mov    %edi,%esi
  102ec4:	b8 02 00 00 00       	mov    $0x2,%eax
  102ec9:	bf 00 10 00 00       	mov    $0x1000,%edi
  102ece:	bb 00 10 00 00       	mov    $0x1000,%ebx
  102ed3:	eb c9                	jmp    102e9e <pmm_alloc_page+0x5e>
  102ed5:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  102edc:	00 
  102edd:	8d 76 00             	lea    0x0(%esi),%esi

00102ee0 <pmm_free_page>:
  102ee0:	53                   	push   %ebx
  102ee1:	8b 44 24 08          	mov    0x8(%esp),%eax
  102ee5:	89 c2                	mov    %eax,%edx
  102ee7:	c1 ea 0c             	shr    $0xc,%edx
  102eea:	74 2c                	je     102f18 <pmm_free_page+0x38>
  102eec:	3b 15 fc 09 11 00    	cmp    0x1109fc,%edx
  102ef2:	73 24                	jae    102f18 <pmm_free_page+0x38>
  102ef4:	8b 0d 00 0a 11 00    	mov    0x110a00,%ecx
  102efa:	c1 e8 11             	shr    $0x11,%eax
  102efd:	89 d3                	mov    %edx,%ebx
  102eff:	83 e3 1f             	and    $0x1f,%ebx
  102f02:	8d 0c 81             	lea    (%ecx,%eax,4),%ecx
  102f05:	8b 01                	mov    (%ecx),%eax
  102f07:	0f a3 d0             	bt     %edx,%eax
  102f0a:	73 0c                	jae    102f18 <pmm_free_page+0x38>
  102f0c:	0f b3 d8             	btr    %ebx,%eax
  102f0f:	83 05 f8 09 11 00 01 	addl   $0x1,0x1109f8
  102f16:	89 01                	mov    %eax,(%ecx)
  102f18:	5b                   	pop    %ebx
  102f19:	c3                   	ret
  102f1a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00102f20 <pmm_alloc_pages>:
  102f20:	55                   	push   %ebp
  102f21:	57                   	push   %edi
  102f22:	56                   	push   %esi
  102f23:	53                   	push   %ebx
  102f24:	83 ec 1c             	sub    $0x1c,%esp
  102f27:	8b 6c 24 30          	mov    0x30(%esp),%ebp
  102f2b:	85 ed                	test   %ebp,%ebp
  102f2d:	74 51                	je     102f80 <pmm_alloc_pages+0x60>
  102f2f:	83 fd 01             	cmp    $0x1,%ebp
  102f32:	0f 84 c8 00 00 00    	je     103000 <pmm_alloc_pages+0xe0>
  102f38:	8b 3d fc 09 11 00    	mov    0x1109fc,%edi
  102f3e:	83 ff 01             	cmp    $0x1,%edi
  102f41:	76 3d                	jbe    102f80 <pmm_alloc_pages+0x60>
  102f43:	8b 15 00 0a 11 00    	mov    0x110a00,%edx
  102f49:	b9 01 00 00 00       	mov    $0x1,%ecx
  102f4e:	31 c0                	xor    %eax,%eax
  102f50:	31 db                	xor    %ebx,%ebx
  102f52:	eb 16                	jmp    102f6a <pmm_alloc_pages+0x4a>
  102f54:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  102f58:	85 c0                	test   %eax,%eax
  102f5a:	74 34                	je     102f90 <pmm_alloc_pages+0x70>
  102f5c:	83 c0 01             	add    $0x1,%eax
  102f5f:	39 c5                	cmp    %eax,%ebp
  102f61:	74 3d                	je     102fa0 <pmm_alloc_pages+0x80>
  102f63:	83 c1 01             	add    $0x1,%ecx
  102f66:	39 f9                	cmp    %edi,%ecx
  102f68:	74 16                	je     102f80 <pmm_alloc_pages+0x60>
  102f6a:	89 ce                	mov    %ecx,%esi
  102f6c:	c1 ee 05             	shr    $0x5,%esi
  102f6f:	8b 34 b2             	mov    (%edx,%esi,4),%esi
  102f72:	0f a3 ce             	bt     %ecx,%esi
  102f75:	73 e1                	jae    102f58 <pmm_alloc_pages+0x38>
  102f77:	83 c1 01             	add    $0x1,%ecx
  102f7a:	31 c0                	xor    %eax,%eax
  102f7c:	39 f9                	cmp    %edi,%ecx
  102f7e:	75 ea                	jne    102f6a <pmm_alloc_pages+0x4a>
  102f80:	83 c4 1c             	add    $0x1c,%esp
  102f83:	31 db                	xor    %ebx,%ebx
  102f85:	89 d8                	mov    %ebx,%eax
  102f87:	5b                   	pop    %ebx
  102f88:	5e                   	pop    %esi
  102f89:	5f                   	pop    %edi
  102f8a:	5d                   	pop    %ebp
  102f8b:	c3                   	ret
  102f8c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  102f90:	89 cb                	mov    %ecx,%ebx
  102f92:	b8 01 00 00 00       	mov    $0x1,%eax
  102f97:	eb ca                	jmp    102f63 <pmm_alloc_pages+0x43>
  102f99:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  102fa0:	8d 2c 18             	lea    (%eax,%ebx,1),%ebp
  102fa3:	39 eb                	cmp    %ebp,%ebx
  102fa5:	73 33                	jae    102fda <pmm_alloc_pages+0xba>
  102fa7:	8b 3d f8 09 11 00    	mov    0x1109f8,%edi
  102fad:	89 d9                	mov    %ebx,%ecx
  102faf:	89 7c 24 0c          	mov    %edi,0xc(%esp)
  102fb3:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  102fb8:	89 ce                	mov    %ecx,%esi
  102fba:	bf 01 00 00 00       	mov    $0x1,%edi
  102fbf:	d3 e7                	shl    %cl,%edi
  102fc1:	c1 ee 05             	shr    $0x5,%esi
  102fc4:	83 c1 01             	add    $0x1,%ecx
  102fc7:	09 3c b2             	or     %edi,(%edx,%esi,4)
  102fca:	39 e9                	cmp    %ebp,%ecx
  102fcc:	75 ea                	jne    102fb8 <pmm_alloc_pages+0x98>
  102fce:	8b 7c 24 0c          	mov    0xc(%esp),%edi
  102fd2:	29 c7                	sub    %eax,%edi
  102fd4:	89 3d f8 09 11 00    	mov    %edi,0x1109f8
  102fda:	c1 e3 0c             	shl    $0xc,%ebx
  102fdd:	83 ec 04             	sub    $0x4,%esp
  102fe0:	c1 e0 0c             	shl    $0xc,%eax
  102fe3:	50                   	push   %eax
  102fe4:	6a 00                	push   $0x0
  102fe6:	53                   	push   %ebx
  102fe7:	e8 74 e5 ff ff       	call   101560 <memset>
  102fec:	83 c4 10             	add    $0x10,%esp
  102fef:	89 d8                	mov    %ebx,%eax
  102ff1:	83 c4 1c             	add    $0x1c,%esp
  102ff4:	5b                   	pop    %ebx
  102ff5:	5e                   	pop    %esi
  102ff6:	5f                   	pop    %edi
  102ff7:	5d                   	pop    %ebp
  102ff8:	c3                   	ret
  102ff9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  103000:	83 c4 1c             	add    $0x1c,%esp
  103003:	5b                   	pop    %ebx
  103004:	5e                   	pop    %esi
  103005:	5f                   	pop    %edi
  103006:	5d                   	pop    %ebp
  103007:	e9 34 fe ff ff       	jmp    102e40 <pmm_alloc_page>
  10300c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00103010 <pmm_free_pages>:
  103010:	55                   	push   %ebp
  103011:	57                   	push   %edi
  103012:	56                   	push   %esi
  103013:	53                   	push   %ebx
  103014:	83 ec 10             	sub    $0x10,%esp
  103017:	8b 74 24 28          	mov    0x28(%esp),%esi
  10301b:	8b 44 24 24          	mov    0x24(%esp),%eax
  10301f:	85 f6                	test   %esi,%esi
  103021:	0f 84 84 00 00 00    	je     1030ab <pmm_free_pages+0x9b>
  103027:	8b 15 fc 09 11 00    	mov    0x1109fc,%edx
  10302d:	c6 44 24 0b 00       	movb   $0x0,0xb(%esp)
  103032:	89 14 24             	mov    %edx,(%esp)
  103035:	8b 15 00 0a 11 00    	mov    0x110a00,%edx
  10303b:	89 54 24 04          	mov    %edx,0x4(%esp)
  10303f:	8b 15 f8 09 11 00    	mov    0x1109f8,%edx
  103045:	89 54 24 0c          	mov    %edx,0xc(%esp)
  103049:	31 d2                	xor    %edx,%edx
  10304b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  103050:	89 c3                	mov    %eax,%ebx
  103052:	c1 eb 0c             	shr    $0xc,%ebx
  103055:	74 38                	je     10308f <pmm_free_pages+0x7f>
  103057:	8b 3c 24             	mov    (%esp),%edi
  10305a:	39 fb                	cmp    %edi,%ebx
  10305c:	73 31                	jae    10308f <pmm_free_pages+0x7f>
  10305e:	8b 7c 24 04          	mov    0x4(%esp),%edi
  103062:	89 c1                	mov    %eax,%ecx
  103064:	c1 e9 11             	shr    $0x11,%ecx
  103067:	8d 3c 8f             	lea    (%edi,%ecx,4),%edi
  10306a:	89 d9                	mov    %ebx,%ecx
  10306c:	8b 2f                	mov    (%edi),%ebp
  10306e:	83 e1 1f             	and    $0x1f,%ecx
  103071:	0f a3 dd             	bt     %ebx,%ebp
  103074:	73 19                	jae    10308f <pmm_free_pages+0x7f>
  103076:	bb 01 00 00 00       	mov    $0x1,%ebx
  10307b:	83 44 24 0c 01       	addl   $0x1,0xc(%esp)
  103080:	d3 e3                	shl    %cl,%ebx
  103082:	c6 44 24 0b 01       	movb   $0x1,0xb(%esp)
  103087:	89 d9                	mov    %ebx,%ecx
  103089:	f7 d1                	not    %ecx
  10308b:	21 e9                	and    %ebp,%ecx
  10308d:	89 0f                	mov    %ecx,(%edi)
  10308f:	83 c2 01             	add    $0x1,%edx
  103092:	05 00 10 00 00       	add    $0x1000,%eax
  103097:	39 d6                	cmp    %edx,%esi
  103099:	75 b5                	jne    103050 <pmm_free_pages+0x40>
  10309b:	80 7c 24 0b 00       	cmpb   $0x0,0xb(%esp)
  1030a0:	74 09                	je     1030ab <pmm_free_pages+0x9b>
  1030a2:	8b 44 24 0c          	mov    0xc(%esp),%eax
  1030a6:	a3 f8 09 11 00       	mov    %eax,0x1109f8
  1030ab:	83 c4 10             	add    $0x10,%esp
  1030ae:	5b                   	pop    %ebx
  1030af:	5e                   	pop    %esi
  1030b0:	5f                   	pop    %edi
  1030b1:	5d                   	pop    %ebp
  1030b2:	c3                   	ret
  1030b3:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1030ba:	00 
  1030bb:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

001030c0 <pmm_free_page_count>:
  1030c0:	a1 f8 09 11 00       	mov    0x1109f8,%eax
  1030c5:	c3                   	ret
  1030c6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1030cd:	00 
  1030ce:	66 90                	xchg   %ax,%ax

001030d0 <pmm_total_page_count>:
  1030d0:	a1 fc 09 11 00       	mov    0x1109fc,%eax
  1030d5:	c3                   	ret
  1030d6:	66 90                	xchg   %ax,%ax
  1030d8:	66 90                	xchg   %ax,%ax
  1030da:	66 90                	xchg   %ax,%ax
  1030dc:	66 90                	xchg   %ax,%ax
  1030de:	66 90                	xchg   %ax,%ax

001030e0 <paging_init>:
  1030e0:	83 ec 10             	sub    $0x10,%esp
  1030e3:	68 00 10 00 00       	push   $0x1000
  1030e8:	6a 00                	push   $0x0
  1030ea:	68 00 10 11 00       	push   $0x111000
  1030ef:	e8 6c e4 ff ff       	call   101560 <memset>
  1030f4:	c7 05 00 10 11 00 83 	movl   $0x83,0x111000
  1030fb:	00 00 00 
  1030fe:	c7 05 00 1c 11 00 83 	movl   $0x83,0x111c00
  103105:	00 00 00 
  103108:	0f 20 e0             	mov    %cr4,%eax
  10310b:	83 c8 10             	or     $0x10,%eax
  10310e:	0f 22 e0             	mov    %eax,%cr4
  103111:	b8 00 10 11 00       	mov    $0x111000,%eax
  103116:	0f 22 d8             	mov    %eax,%cr3
  103119:	0f 20 c0             	mov    %cr0,%eax
  10311c:	0d 00 00 01 80       	or     $0x80010000,%eax
  103121:	0f 22 c0             	mov    %eax,%cr0
  103124:	83 c4 0c             	add    $0xc,%esp
  103127:	68 00 10 11 00       	push   $0x111000
  10312c:	68 f0 99 10 00       	push   $0x1099f0
  103131:	6a 01                	push   $0x1
  103133:	e8 f8 19 00 00       	call   104b30 <klog>
  103138:	83 c4 1c             	add    $0x1c,%esp
  10313b:	c3                   	ret
  10313c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00103140 <paging_map>:
  103140:	55                   	push   %ebp
  103141:	57                   	push   %edi
  103142:	56                   	push   %esi
  103143:	53                   	push   %ebx
  103144:	83 ec 1c             	sub    $0x1c,%esp
  103147:	8b 44 24 30          	mov    0x30(%esp),%eax
  10314b:	8b 74 24 34          	mov    0x34(%esp),%esi
  10314f:	89 c5                	mov    %eax,%ebp
  103151:	89 c7                	mov    %eax,%edi
  103153:	c1 e8 0c             	shr    $0xc,%eax
  103156:	81 e6 00 f0 ff ff    	and    $0xfffff000,%esi
  10315c:	c1 ed 16             	shr    $0x16,%ebp
  10315f:	25 ff 03 00 00       	and    $0x3ff,%eax
  103164:	81 e7 00 f0 ff ff    	and    $0xfffff000,%edi
  10316a:	8b 1c ad 00 10 11 00 	mov    0x111000(,%ebp,4),%ebx
  103171:	89 c1                	mov    %eax,%ecx
  103173:	f6 c3 01             	test   $0x1,%bl
  103176:	74 28                	je     1031a0 <paging_map+0x60>
  103178:	f6 c3 80             	test   $0x80,%bl
  10317b:	75 63                	jne    1031e0 <paging_map+0xa0>
  10317d:	81 e3 00 f0 ff ff    	and    $0xfffff000,%ebx
  103183:	74 5b                	je     1031e0 <paging_map+0xa0>
  103185:	0b 74 24 38          	or     0x38(%esp),%esi
  103189:	83 ce 01             	or     $0x1,%esi
  10318c:	89 34 8b             	mov    %esi,(%ebx,%ecx,4)
  10318f:	0f 01 3f             	invlpg (%edi)
  103192:	83 c4 1c             	add    $0x1c,%esp
  103195:	5b                   	pop    %ebx
  103196:	5e                   	pop    %esi
  103197:	5f                   	pop    %edi
  103198:	5d                   	pop    %ebp
  103199:	c3                   	ret
  10319a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1031a0:	89 44 24 0c          	mov    %eax,0xc(%esp)
  1031a4:	e8 97 fc ff ff       	call   102e40 <pmm_alloc_page>
  1031a9:	89 c3                	mov    %eax,%ebx
  1031ab:	85 c0                	test   %eax,%eax
  1031ad:	74 31                	je     1031e0 <paging_map+0xa0>
  1031af:	83 ec 04             	sub    $0x4,%esp
  1031b2:	68 00 10 00 00       	push   $0x1000
  1031b7:	6a 00                	push   $0x0
  1031b9:	50                   	push   %eax
  1031ba:	e8 a1 e3 ff ff       	call   101560 <memset>
  1031bf:	8b 44 24 48          	mov    0x48(%esp),%eax
  1031c3:	83 c4 10             	add    $0x10,%esp
  1031c6:	8b 4c 24 0c          	mov    0xc(%esp),%ecx
  1031ca:	83 e0 06             	and    $0x6,%eax
  1031cd:	09 d8                	or     %ebx,%eax
  1031cf:	83 c8 01             	or     $0x1,%eax
  1031d2:	89 04 ad 00 10 11 00 	mov    %eax,0x111000(,%ebp,4)
  1031d9:	eb aa                	jmp    103185 <paging_map+0x45>
  1031db:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  1031e0:	83 ec 08             	sub    $0x8,%esp
  1031e3:	57                   	push   %edi
  1031e4:	68 10 9a 10 00       	push   $0x109a10
  1031e9:	e8 22 f8 ff ff       	call   102a10 <kpanic>
  1031ee:	66 90                	xchg   %ax,%ax

001031f0 <paging_unmap>:
  1031f0:	8b 44 24 04          	mov    0x4(%esp),%eax
  1031f4:	89 c1                	mov    %eax,%ecx
  1031f6:	89 c2                	mov    %eax,%edx
  1031f8:	c1 e8 16             	shr    $0x16,%eax
  1031fb:	8b 04 85 00 10 11 00 	mov    0x111000(,%eax,4),%eax
  103202:	c1 ea 0c             	shr    $0xc,%edx
  103205:	81 e1 00 f0 ff ff    	and    $0xfffff000,%ecx
  10320b:	81 e2 ff 03 00 00    	and    $0x3ff,%edx
  103211:	a8 01                	test   $0x1,%al
  103213:	74 15                	je     10322a <paging_unmap+0x3a>
  103215:	a8 80                	test   $0x80,%al
  103217:	75 11                	jne    10322a <paging_unmap+0x3a>
  103219:	25 00 f0 ff ff       	and    $0xfffff000,%eax
  10321e:	74 0a                	je     10322a <paging_unmap+0x3a>
  103220:	c7 04 90 00 00 00 00 	movl   $0x0,(%eax,%edx,4)
  103227:	0f 01 39             	invlpg (%ecx)
  10322a:	c3                   	ret
  10322b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00103230 <paging_flush_tlb>:
  103230:	0f 20 d8             	mov    %cr3,%eax
  103233:	0f 22 d8             	mov    %eax,%cr3
  103236:	c3                   	ret
  103237:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10323e:	00 
  10323f:	90                   	nop

00103240 <paging_invalidate>:
  103240:	8b 44 24 04          	mov    0x4(%esp),%eax
  103244:	0f 01 38             	invlpg (%eax)
  103247:	c3                   	ret
  103248:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10324f:	00 

00103250 <paging_get_phys>:
  103250:	8b 44 24 04          	mov    0x4(%esp),%eax
  103254:	89 c2                	mov    %eax,%edx
  103256:	c1 ea 16             	shr    $0x16,%edx
  103259:	8b 14 95 00 10 11 00 	mov    0x111000(,%edx,4),%edx
  103260:	89 d1                	mov    %edx,%ecx
  103262:	83 e1 01             	and    $0x1,%ecx
  103265:	74 2f                	je     103296 <paging_get_phys+0x46>
  103267:	f6 c2 80             	test   $0x80,%dl
  10326a:	75 34                	jne    1032a0 <paging_get_phys+0x50>
  10326c:	89 c1                	mov    %eax,%ecx
  10326e:	81 e2 00 f0 ff ff    	and    $0xfffff000,%edx
  103274:	c1 e9 0c             	shr    $0xc,%ecx
  103277:	81 e1 ff 03 00 00    	and    $0x3ff,%ecx
  10327d:	8b 14 8a             	mov    (%edx,%ecx,4),%edx
  103280:	89 d1                	mov    %edx,%ecx
  103282:	83 e1 01             	and    $0x1,%ecx
  103285:	74 0f                	je     103296 <paging_get_phys+0x46>
  103287:	81 e2 00 f0 ff ff    	and    $0xfffff000,%edx
  10328d:	25 ff 0f 00 00       	and    $0xfff,%eax
  103292:	89 d1                	mov    %edx,%ecx
  103294:	09 c1                	or     %eax,%ecx
  103296:	89 c8                	mov    %ecx,%eax
  103298:	c3                   	ret
  103299:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  1032a0:	25 ff ff 3f 00       	and    $0x3fffff,%eax
  1032a5:	81 e2 00 00 c0 ff    	and    $0xffc00000,%edx
  1032ab:	89 c1                	mov    %eax,%ecx
  1032ad:	09 d1                	or     %edx,%ecx
  1032af:	89 c8                	mov    %ecx,%eax
  1032b1:	c3                   	ret
  1032b2:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1032b9:	00 
  1032ba:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

001032c0 <paging_get_cr3>:
  1032c0:	b8 00 10 11 00       	mov    $0x111000,%eax
  1032c5:	c3                   	ret
  1032c6:	66 90                	xchg   %ax,%ax
  1032c8:	66 90                	xchg   %ax,%ax
  1032ca:	66 90                	xchg   %ax,%ax
  1032cc:	66 90                	xchg   %ax,%ax
  1032ce:	66 90                	xchg   %ax,%ax

001032d0 <heap_init>:
  1032d0:	53                   	push   %ebx
  1032d1:	b8 00 00 00 d0       	mov    $0xd0000000,%eax
  1032d6:	bb 04 00 00 00       	mov    $0x4,%ebx
  1032db:	83 ec 08             	sub    $0x8,%esp
  1032de:	c7 05 08 20 11 00 00 	movl   $0xd0000000,0x112008
  1032e5:	00 00 d0 
  1032e8:	eb 49                	jmp    103333 <heap_init+0x63>
  1032ea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1032f0:	e8 4b fb ff ff       	call   102e40 <pmm_alloc_page>
  1032f5:	85 c0                	test   %eax,%eax
  1032f7:	74 46                	je     10333f <heap_init+0x6f>
  1032f9:	83 ec 04             	sub    $0x4,%esp
  1032fc:	6a 03                	push   $0x3
  1032fe:	50                   	push   %eax
  1032ff:	ff 35 08 20 11 00    	push   0x112008
  103305:	e8 36 fe ff ff       	call   103140 <paging_map>
  10330a:	a1 08 20 11 00       	mov    0x112008,%eax
  10330f:	8b 0d 00 20 11 00    	mov    0x112000,%ecx
  103315:	83 c4 10             	add    $0x10,%esp
  103318:	05 00 10 00 00       	add    $0x1000,%eax
  10331d:	8d 91 00 10 00 00    	lea    0x1000(%ecx),%edx
  103323:	a3 08 20 11 00       	mov    %eax,0x112008
  103328:	89 15 00 20 11 00    	mov    %edx,0x112000
  10332e:	83 eb 01             	sub    $0x1,%ebx
  103331:	74 1d                	je     103350 <heap_init+0x80>
  103333:	05 00 10 00 00       	add    $0x1000,%eax
  103338:	3d 00 00 00 e0       	cmp    $0xe0000000,%eax
  10333d:	76 b1                	jbe    1032f0 <heap_init+0x20>
  10333f:	83 ec 0c             	sub    $0xc,%esp
  103342:	68 3c 9a 10 00       	push   $0x109a3c
  103347:	e8 c4 f6 ff ff       	call   102a10 <kpanic>
  10334c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  103350:	81 c1 f0 0f 00 00    	add    $0xff0,%ecx
  103356:	c1 ea 0a             	shr    $0xa,%edx
  103359:	c7 05 0c 20 11 00 00 	movl   $0xd0000000,0x11200c
  103360:	00 00 d0 
  103363:	c7 05 00 00 00 d0 ef 	movl   $0xdeadbeef,0xd0000000
  10336a:	be ad de 
  10336d:	89 0d 04 00 00 d0    	mov    %ecx,0xd0000004
  103373:	c6 05 08 00 00 d0 01 	movb   $0x1,0xd0000008
  10337a:	c7 05 0c 00 00 d0 00 	movl   $0x0,0xd000000c
  103381:	00 00 00 
  103384:	52                   	push   %edx
  103385:	68 00 00 00 d0       	push   $0xd0000000
  10338a:	68 68 9a 10 00       	push   $0x109a68
  10338f:	6a 01                	push   $0x1
  103391:	e8 9a 17 00 00       	call   104b30 <klog>
  103396:	83 c4 18             	add    $0x18,%esp
  103399:	5b                   	pop    %ebx
  10339a:	c3                   	ret
  10339b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

001033a0 <kmalloc>:
  1033a0:	55                   	push   %ebp
  1033a1:	57                   	push   %edi
  1033a2:	56                   	push   %esi
  1033a3:	53                   	push   %ebx
  1033a4:	83 ec 0c             	sub    $0xc,%esp
  1033a7:	8b 74 24 20          	mov    0x20(%esp),%esi
  1033ab:	85 f6                	test   %esi,%esi
  1033ad:	0f 84 9e 00 00 00    	je     103451 <kmalloc+0xb1>
  1033b3:	8b 15 0c 20 11 00    	mov    0x11200c,%edx
  1033b9:	83 c6 07             	add    $0x7,%esi
  1033bc:	83 e6 f8             	and    $0xfffffff8,%esi
  1033bf:	85 d2                	test   %edx,%edx
  1033c1:	74 1d                	je     1033e0 <kmalloc+0x40>
  1033c3:	89 d0                	mov    %edx,%eax
  1033c5:	8d 76 00             	lea    0x0(%esi),%esi
  1033c8:	80 78 08 00          	cmpb   $0x0,0x8(%eax)
  1033cc:	74 0b                	je     1033d9 <kmalloc+0x39>
  1033ce:	8b 48 04             	mov    0x4(%eax),%ecx
  1033d1:	39 f1                	cmp    %esi,%ecx
  1033d3:	0f 83 d7 00 00 00    	jae    1034b0 <kmalloc+0x110>
  1033d9:	8b 40 0c             	mov    0xc(%eax),%eax
  1033dc:	85 c0                	test   %eax,%eax
  1033de:	75 e8                	jne    1033c8 <kmalloc+0x28>
  1033e0:	b8 00 10 00 00       	mov    $0x1000,%eax
  1033e5:	8d 7e 10             	lea    0x10(%esi),%edi
  1033e8:	8b 2d 08 20 11 00    	mov    0x112008,%ebp
  1033ee:	39 c7                	cmp    %eax,%edi
  1033f0:	0f 42 f8             	cmovb  %eax,%edi
  1033f3:	89 e8                	mov    %ebp,%eax
  1033f5:	81 c7 ff 0f 00 00    	add    $0xfff,%edi
  1033fb:	c1 ef 0c             	shr    $0xc,%edi
  1033fe:	74 66                	je     103466 <kmalloc+0xc6>
  103400:	31 db                	xor    %ebx,%ebx
  103402:	eb 41                	jmp    103445 <kmalloc+0xa5>
  103404:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  103408:	e8 33 fa ff ff       	call   102e40 <pmm_alloc_page>
  10340d:	85 c0                	test   %eax,%eax
  10340f:	74 40                	je     103451 <kmalloc+0xb1>
  103411:	83 ec 04             	sub    $0x4,%esp
  103414:	83 c3 01             	add    $0x1,%ebx
  103417:	6a 03                	push   $0x3
  103419:	50                   	push   %eax
  10341a:	ff 35 08 20 11 00    	push   0x112008
  103420:	e8 1b fd ff ff       	call   103140 <paging_map>
  103425:	a1 08 20 11 00       	mov    0x112008,%eax
  10342a:	83 c4 10             	add    $0x10,%esp
  10342d:	81 05 00 20 11 00 00 	addl   $0x1000,0x112000
  103434:	10 00 00 
  103437:	05 00 10 00 00       	add    $0x1000,%eax
  10343c:	a3 08 20 11 00       	mov    %eax,0x112008
  103441:	39 df                	cmp    %ebx,%edi
  103443:	74 1b                	je     103460 <kmalloc+0xc0>
  103445:	05 00 10 00 00       	add    $0x1000,%eax
  10344a:	3d 00 00 00 e0       	cmp    $0xe0000000,%eax
  10344f:	76 b7                	jbe    103408 <kmalloc+0x68>
  103451:	31 c0                	xor    %eax,%eax
  103453:	83 c4 0c             	add    $0xc,%esp
  103456:	5b                   	pop    %ebx
  103457:	5e                   	pop    %esi
  103458:	5f                   	pop    %edi
  103459:	5d                   	pop    %ebp
  10345a:	c3                   	ret
  10345b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  103460:	8b 15 0c 20 11 00    	mov    0x11200c,%edx
  103466:	b9 f0 ff ff ff       	mov    $0xfffffff0,%ecx
  10346b:	c7 45 00 ef be ad de 	movl   $0xdeadbeef,0x0(%ebp)
  103472:	29 e9                	sub    %ebp,%ecx
  103474:	c6 45 08 01          	movb   $0x1,0x8(%ebp)
  103478:	01 c8                	add    %ecx,%eax
  10347a:	c7 45 0c 00 00 00 00 	movl   $0x0,0xc(%ebp)
  103481:	89 45 04             	mov    %eax,0x4(%ebp)
  103484:	85 d2                	test   %edx,%edx
  103486:	75 18                	jne    1034a0 <kmalloc+0x100>
  103488:	89 2d 0c 20 11 00    	mov    %ebp,0x11200c
  10348e:	85 f6                	test   %esi,%esi
  103490:	0f 85 1d ff ff ff    	jne    1033b3 <kmalloc+0x13>
  103496:	31 c0                	xor    %eax,%eax
  103498:	eb b9                	jmp    103453 <kmalloc+0xb3>
  10349a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1034a0:	89 d0                	mov    %edx,%eax
  1034a2:	8b 52 0c             	mov    0xc(%edx),%edx
  1034a5:	85 d2                	test   %edx,%edx
  1034a7:	75 f7                	jne    1034a0 <kmalloc+0x100>
  1034a9:	89 68 0c             	mov    %ebp,0xc(%eax)
  1034ac:	eb e0                	jmp    10348e <kmalloc+0xee>
  1034ae:	66 90                	xchg   %ax,%ax
  1034b0:	89 ca                	mov    %ecx,%edx
  1034b2:	29 f2                	sub    %esi,%edx
  1034b4:	8d 5a f0             	lea    -0x10(%edx),%ebx
  1034b7:	83 fb 1f             	cmp    $0x1f,%ebx
  1034ba:	76 1f                	jbe    1034db <kmalloc+0x13b>
  1034bc:	8d 54 30 10          	lea    0x10(%eax,%esi,1),%edx
  1034c0:	c6 42 08 01          	movb   $0x1,0x8(%edx)
  1034c4:	8b 48 0c             	mov    0xc(%eax),%ecx
  1034c7:	c7 02 ef be ad de    	movl   $0xdeadbeef,(%edx)
  1034cd:	89 4a 0c             	mov    %ecx,0xc(%edx)
  1034d0:	89 f1                	mov    %esi,%ecx
  1034d2:	89 5a 04             	mov    %ebx,0x4(%edx)
  1034d5:	89 70 04             	mov    %esi,0x4(%eax)
  1034d8:	89 50 0c             	mov    %edx,0xc(%eax)
  1034db:	c6 40 08 00          	movb   $0x0,0x8(%eax)
  1034df:	83 c0 10             	add    $0x10,%eax
  1034e2:	c7 40 f0 fe ca ef be 	movl   $0xbeefcafe,-0x10(%eax)
  1034e9:	01 0d 04 20 11 00    	add    %ecx,0x112004
  1034ef:	83 c4 0c             	add    $0xc,%esp
  1034f2:	5b                   	pop    %ebx
  1034f3:	5e                   	pop    %esi
  1034f4:	5f                   	pop    %edi
  1034f5:	5d                   	pop    %ebp
  1034f6:	c3                   	ret
  1034f7:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1034fe:	00 
  1034ff:	90                   	nop

00103500 <kzalloc>:
  103500:	56                   	push   %esi
  103501:	53                   	push   %ebx
  103502:	83 ec 10             	sub    $0x10,%esp
  103505:	8b 74 24 1c          	mov    0x1c(%esp),%esi
  103509:	56                   	push   %esi
  10350a:	e8 91 fe ff ff       	call   1033a0 <kmalloc>
  10350f:	83 c4 10             	add    $0x10,%esp
  103512:	89 c3                	mov    %eax,%ebx
  103514:	85 c0                	test   %eax,%eax
  103516:	74 0f                	je     103527 <kzalloc+0x27>
  103518:	83 ec 04             	sub    $0x4,%esp
  10351b:	56                   	push   %esi
  10351c:	6a 00                	push   $0x0
  10351e:	50                   	push   %eax
  10351f:	e8 3c e0 ff ff       	call   101560 <memset>
  103524:	83 c4 10             	add    $0x10,%esp
  103527:	83 c4 04             	add    $0x4,%esp
  10352a:	89 d8                	mov    %ebx,%eax
  10352c:	5b                   	pop    %ebx
  10352d:	5e                   	pop    %esi
  10352e:	c3                   	ret
  10352f:	90                   	nop

00103530 <kmalloc_aligned>:
  103530:	53                   	push   %ebx
  103531:	83 ec 14             	sub    $0x14,%esp
  103534:	8b 5c 24 20          	mov    0x20(%esp),%ebx
  103538:	8d 43 04             	lea    0x4(%ebx),%eax
  10353b:	03 44 24 1c          	add    0x1c(%esp),%eax
  10353f:	50                   	push   %eax
  103540:	e8 5b fe ff ff       	call   1033a0 <kmalloc>
  103545:	83 c4 10             	add    $0x10,%esp
  103548:	85 c0                	test   %eax,%eax
  10354a:	74 0d                	je     103559 <kmalloc_aligned+0x29>
  10354c:	8d 54 18 03          	lea    0x3(%eax,%ebx,1),%edx
  103550:	f7 db                	neg    %ebx
  103552:	21 da                	and    %ebx,%edx
  103554:	89 42 fc             	mov    %eax,-0x4(%edx)
  103557:	89 d0                	mov    %edx,%eax
  103559:	83 c4 08             	add    $0x8,%esp
  10355c:	5b                   	pop    %ebx
  10355d:	c3                   	ret
  10355e:	66 90                	xchg   %ax,%ax

00103560 <kfree>:
  103560:	83 ec 0c             	sub    $0xc,%esp
  103563:	8b 44 24 10          	mov    0x10(%esp),%eax
  103567:	85 c0                	test   %eax,%eax
  103569:	74 59                	je     1035c4 <kfree+0x64>
  10356b:	8b 50 f0             	mov    -0x10(%eax),%edx
  10356e:	81 fa fe ca ef be    	cmp    $0xbeefcafe,%edx
  103574:	75 5e                	jne    1035d4 <kfree+0x74>
  103576:	8b 15 0c 20 11 00    	mov    0x11200c,%edx
  10357c:	c6 40 f8 01          	movb   $0x1,-0x8(%eax)
  103580:	c7 40 f0 ef be ad de 	movl   $0xdeadbeef,-0x10(%eax)
  103587:	8b 40 f4             	mov    -0xc(%eax),%eax
  10358a:	29 05 04 20 11 00    	sub    %eax,0x112004
  103590:	85 d2                	test   %edx,%edx
  103592:	74 30                	je     1035c4 <kfree+0x64>
  103594:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  103598:	8b 42 0c             	mov    0xc(%edx),%eax
  10359b:	85 c0                	test   %eax,%eax
  10359d:	74 25                	je     1035c4 <kfree+0x64>
  10359f:	80 7a 08 00          	cmpb   $0x0,0x8(%edx)
  1035a3:	74 2b                	je     1035d0 <kfree+0x70>
  1035a5:	80 78 08 00          	cmpb   $0x0,0x8(%eax)
  1035a9:	74 25                	je     1035d0 <kfree+0x70>
  1035ab:	8b 48 0c             	mov    0xc(%eax),%ecx
  1035ae:	8b 40 04             	mov    0x4(%eax),%eax
  1035b1:	03 42 04             	add    0x4(%edx),%eax
  1035b4:	83 c0 10             	add    $0x10,%eax
  1035b7:	89 4a 0c             	mov    %ecx,0xc(%edx)
  1035ba:	89 42 04             	mov    %eax,0x4(%edx)
  1035bd:	8b 42 0c             	mov    0xc(%edx),%eax
  1035c0:	85 c0                	test   %eax,%eax
  1035c2:	75 db                	jne    10359f <kfree+0x3f>
  1035c4:	83 c4 0c             	add    $0xc,%esp
  1035c7:	c3                   	ret
  1035c8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1035cf:	00 
  1035d0:	89 c2                	mov    %eax,%edx
  1035d2:	eb c4                	jmp    103598 <kfree+0x38>
  1035d4:	51                   	push   %ecx
  1035d5:	50                   	push   %eax
  1035d6:	52                   	push   %edx
  1035d7:	68 90 9a 10 00       	push   $0x109a90
  1035dc:	e8 2f f4 ff ff       	call   102a10 <kpanic>
  1035e1:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1035e8:	00 
  1035e9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

001035f0 <krealloc>:
  1035f0:	56                   	push   %esi
  1035f1:	53                   	push   %ebx
  1035f2:	83 ec 04             	sub    $0x4,%esp
  1035f5:	8b 74 24 10          	mov    0x10(%esp),%esi
  1035f9:	8b 44 24 14          	mov    0x14(%esp),%eax
  1035fd:	85 f6                	test   %esi,%esi
  1035ff:	74 4f                	je     103650 <krealloc+0x60>
  103601:	85 c0                	test   %eax,%eax
  103603:	74 5b                	je     103660 <krealloc+0x70>
  103605:	89 f3                	mov    %esi,%ebx
  103607:	39 46 f4             	cmp    %eax,-0xc(%esi)
  10360a:	72 0c                	jb     103618 <krealloc+0x28>
  10360c:	83 c4 04             	add    $0x4,%esp
  10360f:	89 d8                	mov    %ebx,%eax
  103611:	5b                   	pop    %ebx
  103612:	5e                   	pop    %esi
  103613:	c3                   	ret
  103614:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  103618:	83 ec 0c             	sub    $0xc,%esp
  10361b:	50                   	push   %eax
  10361c:	e8 7f fd ff ff       	call   1033a0 <kmalloc>
  103621:	83 c4 10             	add    $0x10,%esp
  103624:	89 c3                	mov    %eax,%ebx
  103626:	85 c0                	test   %eax,%eax
  103628:	74 e2                	je     10360c <krealloc+0x1c>
  10362a:	83 ec 04             	sub    $0x4,%esp
  10362d:	ff 76 f4             	push   -0xc(%esi)
  103630:	56                   	push   %esi
  103631:	50                   	push   %eax
  103632:	e8 79 df ff ff       	call   1015b0 <memcpy>
  103637:	89 34 24             	mov    %esi,(%esp)
  10363a:	e8 21 ff ff ff       	call   103560 <kfree>
  10363f:	83 c4 10             	add    $0x10,%esp
  103642:	89 d8                	mov    %ebx,%eax
  103644:	83 c4 04             	add    $0x4,%esp
  103647:	5b                   	pop    %ebx
  103648:	5e                   	pop    %esi
  103649:	c3                   	ret
  10364a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  103650:	89 44 24 10          	mov    %eax,0x10(%esp)
  103654:	83 c4 04             	add    $0x4,%esp
  103657:	5b                   	pop    %ebx
  103658:	5e                   	pop    %esi
  103659:	e9 42 fd ff ff       	jmp    1033a0 <kmalloc>
  10365e:	66 90                	xchg   %ax,%ax
  103660:	83 ec 0c             	sub    $0xc,%esp
  103663:	31 db                	xor    %ebx,%ebx
  103665:	56                   	push   %esi
  103666:	e8 f5 fe ff ff       	call   103560 <kfree>
  10366b:	83 c4 10             	add    $0x10,%esp
  10366e:	eb 9c                	jmp    10360c <krealloc+0x1c>

00103670 <heap_used>:
  103670:	a1 04 20 11 00       	mov    0x112004,%eax
  103675:	c3                   	ret
  103676:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10367d:	00 
  10367e:	66 90                	xchg   %ax,%ax

00103680 <heap_available>:
  103680:	a1 00 20 11 00       	mov    0x112000,%eax
  103685:	2b 05 04 20 11 00    	sub    0x112004,%eax
  10368b:	c3                   	ret
  10368c:	66 90                	xchg   %ax,%ax
  10368e:	66 90                	xchg   %ax,%ax

00103690 <slab_cache_create>:
  103690:	55                   	push   %ebp
  103691:	57                   	push   %edi
  103692:	56                   	push   %esi
  103693:	53                   	push   %ebx
  103694:	83 ec 1c             	sub    $0x1c,%esp
  103697:	83 3d 2c 20 11 00 3f 	cmpl   $0x3f,0x11202c
  10369e:	8b 6c 24 30          	mov    0x30(%esp),%ebp
  1036a2:	8b 44 24 34          	mov    0x34(%esp),%eax
  1036a6:	8b 7c 24 38          	mov    0x38(%esp),%edi
  1036aa:	0f 87 d0 00 00 00    	ja     103780 <slab_cache_create+0xf0>
  1036b0:	ba 04 00 00 00       	mov    $0x4,%edx
  1036b5:	39 d0                	cmp    %edx,%eax
  1036b7:	89 d3                	mov    %edx,%ebx
  1036b9:	0f 43 d8             	cmovae %eax,%ebx
  1036bc:	83 ec 0c             	sub    $0xc,%esp
  1036bf:	6a 4c                	push   $0x4c
  1036c1:	e8 3a fe ff ff       	call   103500 <kzalloc>
  1036c6:	83 c4 10             	add    $0x10,%esp
  1036c9:	89 c6                	mov    %eax,%esi
  1036cb:	85 c0                	test   %eax,%eax
  1036cd:	0f 84 ad 00 00 00    	je     103780 <slab_cache_create+0xf0>
  1036d3:	85 ed                	test   %ebp,%ebp
  1036d5:	b8 86 93 10 00       	mov    $0x109386,%eax
  1036da:	0f 44 e8             	cmove  %eax,%ebp
  1036dd:	83 ec 04             	sub    $0x4,%esp
  1036e0:	6a 1f                	push   $0x1f
  1036e2:	55                   	push   %ebp
  1036e3:	56                   	push   %esi
  1036e4:	e8 07 e0 ff ff       	call   1016f0 <strncpy>
  1036e9:	89 5e 20             	mov    %ebx,0x20(%esi)
  1036ec:	83 c4 10             	add    $0x10,%esp
  1036ef:	85 ff                	test   %edi,%edi
  1036f1:	75 7d                	jne    103770 <slab_cache_create+0xe0>
  1036f3:	b8 f8 ff ff ff       	mov    $0xfffffff8,%eax
  1036f8:	bf 08 00 00 00       	mov    $0x8,%edi
  1036fd:	8d 5c 3b ff          	lea    -0x1(%ebx,%edi,1),%ebx
  103701:	31 d2                	xor    %edx,%edx
  103703:	89 7e 24             	mov    %edi,0x24(%esi)
  103706:	b9 02 00 00 00       	mov    $0x2,%ecx
  10370b:	21 c3                	and    %eax,%ebx
  10370d:	b8 00 10 00 00       	mov    $0x1000,%eax
  103712:	f7 f3                	div    %ebx
  103714:	89 5c 24 0c          	mov    %ebx,0xc(%esp)
  103718:	83 f8 07             	cmp    $0x7,%eax
  10371b:	77 73                	ja     103790 <slab_cache_create+0x100>
  10371d:	8d 76 00             	lea    0x0(%esi),%esi
  103720:	89 c8                	mov    %ecx,%eax
  103722:	31 d2                	xor    %edx,%edx
  103724:	89 cd                	mov    %ecx,%ebp
  103726:	c1 e0 0c             	shl    $0xc,%eax
  103729:	f7 74 24 0c          	divl   0xc(%esp)
  10372d:	83 f8 07             	cmp    $0x7,%eax
  103730:	0f 96 c2             	setbe  %dl
  103733:	83 f9 03             	cmp    $0x3,%ecx
  103736:	89 d7                	mov    %edx,%edi
  103738:	0f 96 c2             	setbe  %dl
  10373b:	83 c1 01             	add    $0x1,%ecx
  10373e:	89 fb                	mov    %edi,%ebx
  103740:	84 d3                	test   %dl,%bl
  103742:	75 dc                	jne    103720 <slab_cache_create+0x90>
  103744:	89 6e 2c             	mov    %ebp,0x2c(%esi)
  103747:	89 46 28             	mov    %eax,0x28(%esi)
  10374a:	a1 2c 20 11 00       	mov    0x11202c,%eax
  10374f:	8d 50 01             	lea    0x1(%eax),%edx
  103752:	89 34 85 40 20 11 00 	mov    %esi,0x112040(,%eax,4)
  103759:	89 f0                	mov    %esi,%eax
  10375b:	89 15 2c 20 11 00    	mov    %edx,0x11202c
  103761:	83 c4 1c             	add    $0x1c,%esp
  103764:	5b                   	pop    %ebx
  103765:	5e                   	pop    %esi
  103766:	5f                   	pop    %edi
  103767:	5d                   	pop    %ebp
  103768:	c3                   	ret
  103769:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  103770:	89 f8                	mov    %edi,%eax
  103772:	f7 d8                	neg    %eax
  103774:	eb 87                	jmp    1036fd <slab_cache_create+0x6d>
  103776:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10377d:	00 
  10377e:	66 90                	xchg   %ax,%ax
  103780:	83 c4 1c             	add    $0x1c,%esp
  103783:	31 f6                	xor    %esi,%esi
  103785:	5b                   	pop    %ebx
  103786:	89 f0                	mov    %esi,%eax
  103788:	5e                   	pop    %esi
  103789:	5f                   	pop    %edi
  10378a:	5d                   	pop    %ebp
  10378b:	c3                   	ret
  10378c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  103790:	c7 46 2c 01 00 00 00 	movl   $0x1,0x2c(%esi)
  103797:	eb ae                	jmp    103747 <slab_cache_create+0xb7>
  103799:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

001037a0 <slab_cache_destroy>:
  1037a0:	57                   	push   %edi
  1037a1:	56                   	push   %esi
  1037a2:	53                   	push   %ebx
  1037a3:	8b 7c 24 10          	mov    0x10(%esp),%edi
  1037a7:	8b 5f 38             	mov    0x38(%edi),%ebx
  1037aa:	85 db                	test   %ebx,%ebx
  1037ac:	74 23                	je     1037d1 <slab_cache_destroy+0x31>
  1037ae:	66 90                	xchg   %ax,%ax
  1037b0:	89 de                	mov    %ebx,%esi
  1037b2:	83 ec 08             	sub    $0x8,%esp
  1037b5:	8b 5b 10             	mov    0x10(%ebx),%ebx
  1037b8:	ff 77 2c             	push   0x2c(%edi)
  1037bb:	ff 36                	push   (%esi)
  1037bd:	e8 4e f8 ff ff       	call   103010 <pmm_free_pages>
  1037c2:	89 34 24             	mov    %esi,(%esp)
  1037c5:	e8 96 fd ff ff       	call   103560 <kfree>
  1037ca:	83 c4 10             	add    $0x10,%esp
  1037cd:	85 db                	test   %ebx,%ebx
  1037cf:	75 df                	jne    1037b0 <slab_cache_destroy+0x10>
  1037d1:	89 7c 24 10          	mov    %edi,0x10(%esp)
  1037d5:	5b                   	pop    %ebx
  1037d6:	5e                   	pop    %esi
  1037d7:	5f                   	pop    %edi
  1037d8:	e9 83 fd ff ff       	jmp    103560 <kfree>
  1037dd:	8d 76 00             	lea    0x0(%esi),%esi

001037e0 <slab_alloc>:
  1037e0:	55                   	push   %ebp
  1037e1:	57                   	push   %edi
  1037e2:	56                   	push   %esi
  1037e3:	53                   	push   %ebx
  1037e4:	83 ec 2c             	sub    $0x2c,%esp
  1037e7:	8b 5c 24 40          	mov    0x40(%esp),%ebx
  1037eb:	9c                   	pushf
  1037ec:	5e                   	pop    %esi
  1037ed:	fa                   	cli
  1037ee:	8b 6b 30             	mov    0x30(%ebx),%ebp
  1037f1:	85 ed                	test   %ebp,%ebp
  1037f3:	74 6b                	je     103860 <slab_alloc+0x80>
  1037f5:	8b 45 08             	mov    0x8(%ebp),%eax
  1037f8:	8b 4b 20             	mov    0x20(%ebx),%ecx
  1037fb:	8b 7d 0c             	mov    0xc(%ebp),%edi
  1037fe:	89 44 24 0c          	mov    %eax,0xc(%esp)
  103802:	83 e8 01             	sub    $0x1,%eax
  103805:	8b 17                	mov    (%edi),%edx
  103807:	89 45 08             	mov    %eax,0x8(%ebp)
  10380a:	89 55 0c             	mov    %edx,0xc(%ebp)
  10380d:	83 43 3c 01          	addl   $0x1,0x3c(%ebx)
  103811:	83 53 40 00          	adcl   $0x0,0x40(%ebx)
  103815:	85 c0                	test   %eax,%eax
  103817:	74 2f                	je     103848 <slab_alloc+0x68>
  103819:	8b 44 24 0c          	mov    0xc(%esp),%eax
  10381d:	39 45 04             	cmp    %eax,0x4(%ebp)
  103820:	0f 84 f2 00 00 00    	je     103918 <slab_alloc+0x138>
  103826:	56                   	push   %esi
  103827:	9d                   	popf
  103828:	83 ec 04             	sub    $0x4,%esp
  10382b:	51                   	push   %ecx
  10382c:	6a 00                	push   $0x0
  10382e:	57                   	push   %edi
  10382f:	e8 2c dd ff ff       	call   101560 <memset>
  103834:	83 c4 10             	add    $0x10,%esp
  103837:	83 c4 2c             	add    $0x2c,%esp
  10383a:	89 f8                	mov    %edi,%eax
  10383c:	5b                   	pop    %ebx
  10383d:	5e                   	pop    %esi
  10383e:	5f                   	pop    %edi
  10383f:	5d                   	pop    %ebp
  103840:	c3                   	ret
  103841:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  103848:	39 6b 30             	cmp    %ebp,0x30(%ebx)
  10384b:	0f 84 df 00 00 00    	je     103930 <slab_alloc+0x150>
  103851:	8b 43 34             	mov    0x34(%ebx),%eax
  103854:	89 45 10             	mov    %eax,0x10(%ebp)
  103857:	89 6b 34             	mov    %ebp,0x34(%ebx)
  10385a:	eb ca                	jmp    103826 <slab_alloc+0x46>
  10385c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  103860:	8b 6b 38             	mov    0x38(%ebx),%ebp
  103863:	85 ed                	test   %ebp,%ebp
  103865:	75 8e                	jne    1037f5 <slab_alloc+0x15>
  103867:	83 ec 0c             	sub    $0xc,%esp
  10386a:	6a 14                	push   $0x14
  10386c:	e8 2f fb ff ff       	call   1033a0 <kmalloc>
  103871:	89 44 24 20          	mov    %eax,0x20(%esp)
  103875:	83 c4 10             	add    $0x10,%esp
  103878:	85 c0                	test   %eax,%eax
  10387a:	0f 84 d7 00 00 00    	je     103957 <slab_alloc+0x177>
  103880:	83 ec 0c             	sub    $0xc,%esp
  103883:	ff 73 2c             	push   0x2c(%ebx)
  103886:	e8 95 f6 ff ff       	call   102f20 <pmm_alloc_pages>
  10388b:	83 c4 10             	add    $0x10,%esp
  10388e:	89 c7                	mov    %eax,%edi
  103890:	85 c0                	test   %eax,%eax
  103892:	0f 84 b0 00 00 00    	je     103948 <slab_alloc+0x168>
  103898:	8b 4c 24 10          	mov    0x10(%esp),%ecx
  10389c:	89 c2                	mov    %eax,%edx
  10389e:	89 01                	mov    %eax,(%ecx)
  1038a0:	8b 43 28             	mov    0x28(%ebx),%eax
  1038a3:	c7 41 10 00 00 00 00 	movl   $0x0,0x10(%ecx)
  1038aa:	89 41 04             	mov    %eax,0x4(%ecx)
  1038ad:	89 44 24 0c          	mov    %eax,0xc(%esp)
  1038b1:	89 41 08             	mov    %eax,0x8(%ecx)
  1038b4:	8b 43 24             	mov    0x24(%ebx),%eax
  1038b7:	8b 4b 20             	mov    0x20(%ebx),%ecx
  1038ba:	8d 6c 08 ff          	lea    -0x1(%eax,%ecx,1),%ebp
  1038be:	f7 d8                	neg    %eax
  1038c0:	21 c5                	and    %eax,%ebp
  1038c2:	8b 44 24 0c          	mov    0xc(%esp),%eax
  1038c6:	89 6c 24 14          	mov    %ebp,0x14(%esp)
  1038ca:	31 ed                	xor    %ebp,%ebp
  1038cc:	83 e8 01             	sub    $0x1,%eax
  1038cf:	74 24                	je     1038f5 <slab_alloc+0x115>
  1038d1:	89 7c 24 18          	mov    %edi,0x18(%esp)
  1038d5:	89 4c 24 1c          	mov    %ecx,0x1c(%esp)
  1038d9:	8b 4c 24 14          	mov    0x14(%esp),%ecx
  1038dd:	8d 76 00             	lea    0x0(%esi),%esi
  1038e0:	89 fa                	mov    %edi,%edx
  1038e2:	83 c5 01             	add    $0x1,%ebp
  1038e5:	01 cf                	add    %ecx,%edi
  1038e7:	89 3a                	mov    %edi,(%edx)
  1038e9:	39 e8                	cmp    %ebp,%eax
  1038eb:	75 f3                	jne    1038e0 <slab_alloc+0x100>
  1038ed:	8b 54 24 18          	mov    0x18(%esp),%edx
  1038f1:	8b 4c 24 1c          	mov    0x1c(%esp),%ecx
  1038f5:	8b 6c 24 14          	mov    0x14(%esp),%ebp
  1038f9:	0f af e8             	imul   %eax,%ebp
  1038fc:	c7 04 2a 00 00 00 00 	movl   $0x0,(%edx,%ebp,1)
  103903:	8b 54 24 10          	mov    0x10(%esp),%edx
  103907:	8b 3a                	mov    (%edx),%edi
  103909:	89 d5                	mov    %edx,%ebp
  10390b:	89 7a 0c             	mov    %edi,0xc(%edx)
  10390e:	e9 f2 fe ff ff       	jmp    103805 <slab_alloc+0x25>
  103913:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  103918:	39 6b 38             	cmp    %ebp,0x38(%ebx)
  10391b:	74 23                	je     103940 <slab_alloc+0x160>
  10391d:	8b 43 30             	mov    0x30(%ebx),%eax
  103920:	89 45 10             	mov    %eax,0x10(%ebp)
  103923:	89 6b 30             	mov    %ebp,0x30(%ebx)
  103926:	e9 fb fe ff ff       	jmp    103826 <slab_alloc+0x46>
  10392b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  103930:	8b 45 10             	mov    0x10(%ebp),%eax
  103933:	89 43 30             	mov    %eax,0x30(%ebx)
  103936:	e9 16 ff ff ff       	jmp    103851 <slab_alloc+0x71>
  10393b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  103940:	8b 45 10             	mov    0x10(%ebp),%eax
  103943:	89 43 38             	mov    %eax,0x38(%ebx)
  103946:	eb d5                	jmp    10391d <slab_alloc+0x13d>
  103948:	83 ec 0c             	sub    $0xc,%esp
  10394b:	ff 74 24 1c          	push   0x1c(%esp)
  10394f:	e8 0c fc ff ff       	call   103560 <kfree>
  103954:	83 c4 10             	add    $0x10,%esp
  103957:	56                   	push   %esi
  103958:	9d                   	popf
  103959:	31 ff                	xor    %edi,%edi
  10395b:	e9 d7 fe ff ff       	jmp    103837 <slab_alloc+0x57>

00103960 <slab_free>:
  103960:	55                   	push   %ebp
  103961:	57                   	push   %edi
  103962:	56                   	push   %esi
  103963:	53                   	push   %ebx
  103964:	83 ec 2c             	sub    $0x2c,%esp
  103967:	8b 4c 24 44          	mov    0x44(%esp),%ecx
  10396b:	8b 7c 24 40          	mov    0x40(%esp),%edi
  10396f:	85 c9                	test   %ecx,%ecx
  103971:	0f 84 b4 00 00 00    	je     103a2b <slab_free+0xcb>
  103977:	9c                   	pushf
  103978:	58                   	pop    %eax
  103979:	fa                   	cli
  10397a:	89 44 24 0c          	mov    %eax,0xc(%esp)
  10397e:	8d 47 30             	lea    0x30(%edi),%eax
  103981:	8b 77 2c             	mov    0x2c(%edi),%esi
  103984:	8d 6f 34             	lea    0x34(%edi),%ebp
  103987:	89 44 24 14          	mov    %eax,0x14(%esp)
  10398b:	8d 47 38             	lea    0x38(%edi),%eax
  10398e:	8d 54 24 10          	lea    0x10(%esp),%edx
  103992:	89 44 24 18          	mov    %eax,0x18(%esp)
  103996:	c1 e6 0c             	shl    $0xc,%esi
  103999:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
  1039a0:	00 
  1039a1:	8b 45 00             	mov    0x0(%ebp),%eax
  1039a4:	85 c0                	test   %eax,%eax
  1039a6:	74 1f                	je     1039c7 <slab_free+0x67>
  1039a8:	31 db                	xor    %ebx,%ebx
  1039aa:	eb 06                	jmp    1039b2 <slab_free+0x52>
  1039ac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  1039b0:	89 f8                	mov    %edi,%eax
  1039b2:	8b 38                	mov    (%eax),%edi
  1039b4:	39 f9                	cmp    %edi,%ecx
  1039b6:	72 06                	jb     1039be <slab_free+0x5e>
  1039b8:	01 f7                	add    %esi,%edi
  1039ba:	39 f9                	cmp    %edi,%ecx
  1039bc:	72 32                	jb     1039f0 <slab_free+0x90>
  1039be:	8b 78 10             	mov    0x10(%eax),%edi
  1039c1:	89 c3                	mov    %eax,%ebx
  1039c3:	85 ff                	test   %edi,%edi
  1039c5:	75 e9                	jne    1039b0 <slab_free+0x50>
  1039c7:	8b 6a 04             	mov    0x4(%edx),%ebp
  1039ca:	83 c2 04             	add    $0x4,%edx
  1039cd:	85 ed                	test   %ebp,%ebp
  1039cf:	75 d0                	jne    1039a1 <slab_free+0x41>
  1039d1:	8b 7c 24 40          	mov    0x40(%esp),%edi
  1039d5:	8b 44 24 0c          	mov    0xc(%esp),%eax
  1039d9:	50                   	push   %eax
  1039da:	9d                   	popf
  1039db:	83 ec 04             	sub    $0x4,%esp
  1039de:	57                   	push   %edi
  1039df:	51                   	push   %ecx
  1039e0:	68 b0 9a 10 00       	push   $0x109ab0
  1039e5:	e8 26 f0 ff ff       	call   102a10 <kpanic>
  1039ea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1039f0:	8b 7c 24 40          	mov    0x40(%esp),%edi
  1039f4:	8b 50 10             	mov    0x10(%eax),%edx
  1039f7:	85 db                	test   %ebx,%ebx
  1039f9:	74 38                	je     103a33 <slab_free+0xd3>
  1039fb:	89 53 10             	mov    %edx,0x10(%ebx)
  1039fe:	8b 50 0c             	mov    0xc(%eax),%edx
  103a01:	89 11                	mov    %edx,(%ecx)
  103a03:	8b 50 08             	mov    0x8(%eax),%edx
  103a06:	89 48 0c             	mov    %ecx,0xc(%eax)
  103a09:	83 c2 01             	add    $0x1,%edx
  103a0c:	89 50 08             	mov    %edx,0x8(%eax)
  103a0f:	83 47 44 01          	addl   $0x1,0x44(%edi)
  103a13:	83 57 48 00          	adcl   $0x0,0x48(%edi)
  103a17:	3b 50 04             	cmp    0x4(%eax),%edx
  103a1a:	74 24                	je     103a40 <slab_free+0xe0>
  103a1c:	8b 57 30             	mov    0x30(%edi),%edx
  103a1f:	89 50 10             	mov    %edx,0x10(%eax)
  103a22:	89 47 30             	mov    %eax,0x30(%edi)
  103a25:	8b 44 24 0c          	mov    0xc(%esp),%eax
  103a29:	50                   	push   %eax
  103a2a:	9d                   	popf
  103a2b:	83 c4 2c             	add    $0x2c,%esp
  103a2e:	5b                   	pop    %ebx
  103a2f:	5e                   	pop    %esi
  103a30:	5f                   	pop    %edi
  103a31:	5d                   	pop    %ebp
  103a32:	c3                   	ret
  103a33:	89 55 00             	mov    %edx,0x0(%ebp)
  103a36:	eb c6                	jmp    1039fe <slab_free+0x9e>
  103a38:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  103a3f:	00 
  103a40:	8b 57 38             	mov    0x38(%edi),%edx
  103a43:	89 50 10             	mov    %edx,0x10(%eax)
  103a46:	89 47 38             	mov    %eax,0x38(%edi)
  103a49:	eb da                	jmp    103a25 <slab_free+0xc5>
  103a4b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00103a50 <slab_init>:
  103a50:	83 ec 10             	sub    $0x10,%esp
  103a53:	68 00 01 00 00       	push   $0x100
  103a58:	6a 00                	push   $0x0
  103a5a:	68 40 20 11 00       	push   $0x112040
  103a5f:	e8 fc da ff ff       	call   101560 <memset>
  103a64:	83 c4 0c             	add    $0xc,%esp
  103a67:	c7 05 2c 20 11 00 00 	movl   $0x0,0x11202c
  103a6e:	00 00 00 
  103a71:	6a 08                	push   $0x8
  103a73:	68 00 01 00 00       	push   $0x100
  103a78:	68 1b 93 10 00       	push   $0x10931b
  103a7d:	e8 0e fc ff ff       	call   103690 <slab_cache_create>
  103a82:	83 c4 0c             	add    $0xc,%esp
  103a85:	6a 08                	push   $0x8
  103a87:	6a 40                	push   $0x40
  103a89:	68 22 93 10 00       	push   $0x109322
  103a8e:	a3 28 20 11 00       	mov    %eax,0x112028
  103a93:	e8 f8 fb ff ff       	call   103690 <slab_cache_create>
  103a98:	83 c4 0c             	add    $0xc,%esp
  103a9b:	6a 08                	push   $0x8
  103a9d:	68 80 00 00 00       	push   $0x80
  103aa2:	68 28 93 10 00       	push   $0x109328
  103aa7:	a3 24 20 11 00       	mov    %eax,0x112024
  103aac:	e8 df fb ff ff       	call   103690 <slab_cache_create>
  103ab1:	83 c4 0c             	add    $0xc,%esp
  103ab4:	ff 35 2c 20 11 00    	push   0x11202c
  103aba:	68 e4 9a 10 00       	push   $0x109ae4
  103abf:	6a 01                	push   $0x1
  103ac1:	a3 20 20 11 00       	mov    %eax,0x112020
  103ac6:	e8 65 10 00 00       	call   104b30 <klog>
  103acb:	83 c4 1c             	add    $0x1c,%esp
  103ace:	c3                   	ret
  103acf:	90                   	nop

00103ad0 <slab_dump>:
  103ad0:	53                   	push   %ebx
  103ad1:	83 ec 10             	sub    $0x10,%esp
  103ad4:	68 30 93 10 00       	push   $0x109330
  103ad9:	6a 01                	push   $0x1
  103adb:	e8 50 10 00 00       	call   104b30 <klog>
  103ae0:	a1 2c 20 11 00       	mov    0x11202c,%eax
  103ae5:	83 c4 10             	add    $0x10,%esp
  103ae8:	85 c0                	test   %eax,%eax
  103aea:	74 3f                	je     103b2b <slab_dump+0x5b>
  103aec:	31 db                	xor    %ebx,%ebx
  103aee:	66 90                	xchg   %ax,%ax
  103af0:	8b 04 9d 40 20 11 00 	mov    0x112040(,%ebx,4),%eax
  103af7:	85 c0                	test   %eax,%eax
  103af9:	74 25                	je     103b20 <slab_dump+0x50>
  103afb:	83 ec 0c             	sub    $0xc,%esp
  103afe:	ff 70 48             	push   0x48(%eax)
  103b01:	ff 70 44             	push   0x44(%eax)
  103b04:	ff 70 40             	push   0x40(%eax)
  103b07:	ff 70 3c             	push   0x3c(%eax)
  103b0a:	ff 70 28             	push   0x28(%eax)
  103b0d:	ff 70 20             	push   0x20(%eax)
  103b10:	50                   	push   %eax
  103b11:	68 08 9b 10 00       	push   $0x109b08
  103b16:	6a 01                	push   $0x1
  103b18:	e8 13 10 00 00       	call   104b30 <klog>
  103b1d:	83 c4 30             	add    $0x30,%esp
  103b20:	83 c3 01             	add    $0x1,%ebx
  103b23:	3b 1d 2c 20 11 00    	cmp    0x11202c,%ebx
  103b29:	72 c5                	jb     103af0 <slab_dump+0x20>
  103b2b:	83 c4 08             	add    $0x8,%esp
  103b2e:	5b                   	pop    %ebx
  103b2f:	c3                   	ret

00103b30 <proc_init>:
  103b30:	83 ec 10             	sub    $0x10,%esp
  103b33:	68 00 59 00 00       	push   $0x5900
  103b38:	6a 00                	push   $0x0
  103b3a:	68 60 21 11 00       	push   $0x112160
  103b3f:	e8 1c da ff ff       	call   101560 <memset>
  103b44:	83 c4 0c             	add    $0xc,%esp
  103b47:	c7 05 40 21 11 00 00 	movl   $0x0,0x112140
  103b4e:	00 00 00 
  103b51:	6a 40                	push   $0x40
  103b53:	68 40 9b 10 00       	push   $0x109b40
  103b58:	6a 01                	push   $0x1
  103b5a:	e8 d1 0f 00 00       	call   104b30 <klog>
  103b5f:	83 c4 1c             	add    $0x1c,%esp
  103b62:	c3                   	ret
  103b63:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  103b6a:	00 
  103b6b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00103b70 <proc_create>:
  103b70:	55                   	push   %ebp
  103b71:	b8 70 21 11 00       	mov    $0x112170,%eax
  103b76:	57                   	push   %edi
  103b77:	56                   	push   %esi
  103b78:	53                   	push   %ebx
  103b79:	31 db                	xor    %ebx,%ebx
  103b7b:	83 ec 1c             	sub    $0x1c,%esp
  103b7e:	8b 4c 24 30          	mov    0x30(%esp),%ecx
  103b82:	eb 15                	jmp    103b99 <proc_create+0x29>
  103b84:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  103b88:	83 c3 01             	add    $0x1,%ebx
  103b8b:	05 64 01 00 00       	add    $0x164,%eax
  103b90:	83 fb 40             	cmp    $0x40,%ebx
  103b93:	0f 84 11 01 00 00    	je     103caa <proc_create+0x13a>
  103b99:	8b 10                	mov    (%eax),%edx
  103b9b:	85 d2                	test   %edx,%edx
  103b9d:	75 e9                	jne    103b88 <proc_create+0x18>
  103b9f:	69 fb 64 01 00 00    	imul   $0x164,%ebx,%edi
  103ba5:	89 4c 24 30          	mov    %ecx,0x30(%esp)
  103ba9:	83 ec 04             	sub    $0x4,%esp
  103bac:	89 54 24 10          	mov    %edx,0x10(%esp)
  103bb0:	68 64 01 00 00       	push   $0x164
  103bb5:	8d b7 60 21 11 00    	lea    0x112160(%edi),%esi
  103bbb:	6a 00                	push   $0x0
  103bbd:	56                   	push   %esi
  103bbe:	e8 9d d9 ff ff       	call   101560 <memset>
  103bc3:	83 c4 10             	add    $0x10,%esp
  103bc6:	8b 54 24 0c          	mov    0xc(%esp),%edx
  103bca:	8b 4c 24 30          	mov    0x30(%esp),%ecx
  103bce:	a1 00 b0 10 00       	mov    0x10b000,%eax
  103bd3:	8d 68 01             	lea    0x1(%eax),%ebp
  103bd6:	89 87 60 21 11 00    	mov    %eax,0x112160(%edi)
  103bdc:	89 2d 00 b0 10 00    	mov    %ebp,0x10b000
  103be2:	8b 2d 40 21 11 00    	mov    0x112140,%ebp
  103be8:	85 ed                	test   %ebp,%ebp
  103bea:	74 03                	je     103bef <proc_create+0x7f>
  103bec:	8b 55 00             	mov    0x0(%ebp),%edx
  103bef:	69 eb 64 01 00 00    	imul   $0x164,%ebx,%ebp
  103bf5:	89 95 64 21 11 00    	mov    %edx,0x112164(%ebp)
  103bfb:	8d 97 a4 22 11 00    	lea    0x1122a4(%edi),%edx
  103c01:	89 85 68 21 11 00    	mov    %eax,0x112168(%ebp)
  103c07:	89 85 6c 21 11 00    	mov    %eax,0x11216c(%ebp)
  103c0d:	b8 48 93 10 00       	mov    $0x109348,%eax
  103c12:	c7 85 70 21 11 00 02 	movl   $0x2,0x112170(%ebp)
  103c19:	00 00 00 
  103c1c:	81 c5 60 21 11 00    	add    $0x112160,%ebp
  103c22:	85 c9                	test   %ecx,%ecx
  103c24:	0f 44 c8             	cmove  %eax,%ecx
  103c27:	83 ec 04             	sub    $0x4,%esp
  103c2a:	6a 1f                	push   $0x1f
  103c2c:	51                   	push   %ecx
  103c2d:	52                   	push   %edx
  103c2e:	89 54 24 1c          	mov    %edx,0x1c(%esp)
  103c32:	e8 b9 da ff ff       	call   1016f0 <strncpy>
  103c37:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
  103c3e:	e8 dd f2 ff ff       	call   102f20 <pmm_alloc_pages>
  103c43:	8b 54 24 1c          	mov    0x1c(%esp),%edx
  103c47:	83 c4 10             	add    $0x10,%esp
  103c4a:	89 85 ac 00 00 00    	mov    %eax,0xac(%ebp)
  103c50:	05 00 20 00 00       	add    $0x2000,%eax
  103c55:	89 85 a8 00 00 00    	mov    %eax,0xa8(%ebp)
  103c5b:	8d 87 88 21 11 00    	lea    0x112188(%edi),%eax
  103c61:	81 c7 08 22 11 00    	add    $0x112208,%edi
  103c67:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  103c6e:	00 
  103c6f:	90                   	nop
  103c70:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
  103c76:	83 c0 08             	add    $0x8,%eax
  103c79:	c7 40 fc 00 00 00 00 	movl   $0x0,-0x4(%eax)
  103c80:	39 f8                	cmp    %edi,%eax
  103c82:	75 ec                	jne    103c70 <proc_create+0x100>
  103c84:	69 db 64 01 00 00    	imul   $0x164,%ebx,%ebx
  103c8a:	52                   	push   %edx
  103c8b:	ff b3 60 21 11 00    	push   0x112160(%ebx)
  103c91:	68 50 93 10 00       	push   $0x109350
  103c96:	6a 01                	push   $0x1
  103c98:	e8 93 0e 00 00       	call   104b30 <klog>
  103c9d:	83 c4 10             	add    $0x10,%esp
  103ca0:	89 f0                	mov    %esi,%eax
  103ca2:	83 c4 1c             	add    $0x1c,%esp
  103ca5:	5b                   	pop    %ebx
  103ca6:	5e                   	pop    %esi
  103ca7:	5f                   	pop    %edi
  103ca8:	5d                   	pop    %ebp
  103ca9:	c3                   	ret
  103caa:	83 c4 1c             	add    $0x1c,%esp
  103cad:	31 f6                	xor    %esi,%esi
  103caf:	5b                   	pop    %ebx
  103cb0:	89 f0                	mov    %esi,%eax
  103cb2:	5e                   	pop    %esi
  103cb3:	5f                   	pop    %edi
  103cb4:	5d                   	pop    %ebp
  103cb5:	c3                   	ret
  103cb6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  103cbd:	00 
  103cbe:	66 90                	xchg   %ax,%ax

00103cc0 <proc_signal>:
  103cc0:	8b 4c 24 08          	mov    0x8(%esp),%ecx
  103cc4:	8b 44 24 04          	mov    0x4(%esp),%eax
  103cc8:	8d 51 ff             	lea    -0x1(%ecx),%edx
  103ccb:	83 fa 1e             	cmp    $0x1e,%edx
  103cce:	77 15                	ja     103ce5 <proc_signal+0x25>
  103cd0:	85 c0                	test   %eax,%eax
  103cd2:	74 11                	je     103ce5 <proc_signal+0x25>
  103cd4:	ba 01 00 00 00       	mov    $0x1,%edx
  103cd9:	d3 e2                	shl    %cl,%edx
  103cdb:	89 d1                	mov    %edx,%ecx
  103cdd:	23 48 24             	and    0x24(%eax),%ecx
  103ce0:	75 03                	jne    103ce5 <proc_signal+0x25>
  103ce2:	09 50 20             	or     %edx,0x20(%eax)
  103ce5:	c3                   	ret
  103ce6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  103ced:	00 
  103cee:	66 90                	xchg   %ax,%ax

00103cf0 <proc_set_sighandler>:
  103cf0:	8b 44 24 08          	mov    0x8(%esp),%eax
  103cf4:	8b 54 24 04          	mov    0x4(%esp),%edx
  103cf8:	8d 48 ff             	lea    -0x1(%eax),%ecx
  103cfb:	83 f9 1e             	cmp    $0x1e,%ecx
  103cfe:	77 16                	ja     103d16 <proc_set_sighandler+0x26>
  103d00:	85 d2                	test   %edx,%edx
  103d02:	74 12                	je     103d16 <proc_set_sighandler+0x26>
  103d04:	83 f8 09             	cmp    $0x9,%eax
  103d07:	74 0d                	je     103d16 <proc_set_sighandler+0x26>
  103d09:	83 f8 13             	cmp    $0x13,%eax
  103d0c:	74 08                	je     103d16 <proc_set_sighandler+0x26>
  103d0e:	8b 4c 24 0c          	mov    0xc(%esp),%ecx
  103d12:	89 4c 82 28          	mov    %ecx,0x28(%edx,%eax,4)
  103d16:	c3                   	ret
  103d17:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  103d1e:	00 
  103d1f:	90                   	nop

00103d20 <proc_handle_signals>:
  103d20:	57                   	push   %edi
  103d21:	56                   	push   %esi
  103d22:	53                   	push   %ebx
  103d23:	8b 7c 24 10          	mov    0x10(%esp),%edi
  103d27:	85 ff                	test   %edi,%edi
  103d29:	74 65                	je     103d90 <proc_handle_signals+0x70>
  103d2b:	8b 57 20             	mov    0x20(%edi),%edx
  103d2e:	85 d2                	test   %edx,%edx
  103d30:	74 5e                	je     103d90 <proc_handle_signals+0x70>
  103d32:	bb 81 00 00 00       	mov    $0x81,%ebx
  103d37:	be 01 00 00 00       	mov    $0x1,%esi
  103d3c:	eb 19                	jmp    103d57 <proc_handle_signals+0x37>
  103d3e:	66 90                	xchg   %ax,%ax
  103d40:	83 ec 0c             	sub    $0xc,%esp
  103d43:	51                   	push   %ecx
  103d44:	ff d0                	call   *%eax
  103d46:	83 c4 10             	add    $0x10,%esp
  103d49:	83 c3 01             	add    $0x1,%ebx
  103d4c:	81 fb a0 00 00 00    	cmp    $0xa0,%ebx
  103d52:	74 3c                	je     103d90 <proc_handle_signals+0x70>
  103d54:	8b 57 20             	mov    0x20(%edi),%edx
  103d57:	8d 4b 80             	lea    -0x80(%ebx),%ecx
  103d5a:	89 f0                	mov    %esi,%eax
  103d5c:	d3 e0                	shl    %cl,%eax
  103d5e:	85 d0                	test   %edx,%eax
  103d60:	74 e7                	je     103d49 <proc_handle_signals+0x29>
  103d62:	f7 d0                	not    %eax
  103d64:	21 d0                	and    %edx,%eax
  103d66:	89 47 20             	mov    %eax,0x20(%edi)
  103d69:	8b 84 9f 28 fe ff ff 	mov    -0x1d8(%edi,%ebx,4),%eax
  103d70:	83 f8 01             	cmp    $0x1,%eax
  103d73:	74 d4                	je     103d49 <proc_handle_signals+0x29>
  103d75:	85 c0                	test   %eax,%eax
  103d77:	75 c7                	jne    103d40 <proc_handle_signals+0x20>
  103d79:	8d 83 77 ff ff ff    	lea    -0x89(%ebx),%eax
  103d7f:	83 f8 0a             	cmp    $0xa,%eax
  103d82:	77 1c                	ja     103da0 <proc_handle_signals+0x80>
  103d84:	ff 24 85 60 a6 10 00 	jmp    *0x10a660(,%eax,4)
  103d8b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  103d90:	5b                   	pop    %ebx
  103d91:	5e                   	pop    %esi
  103d92:	5f                   	pop    %edi
  103d93:	c3                   	ret
  103d94:	bb 94 00 00 00       	mov    $0x94,%ebx
  103d99:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  103da0:	c7 47 10 04 00 00 00 	movl   $0x4,0x10(%edi)
  103da7:	89 5f 14             	mov    %ebx,0x14(%edi)
  103daa:	eb 9d                	jmp    103d49 <proc_handle_signals+0x29>
  103dac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  103db0:	89 5f 14             	mov    %ebx,0x14(%edi)
  103db3:	83 c3 01             	add    $0x1,%ebx
  103db6:	c7 47 10 04 00 00 00 	movl   $0x4,0x10(%edi)
  103dbd:	eb 95                	jmp    103d54 <proc_handle_signals+0x34>
  103dbf:	8b 47 20             	mov    0x20(%edi),%eax
  103dc2:	a9 00 00 04 00       	test   $0x40000,%eax
  103dc7:	0f 84 ad 00 00 00    	je     103e7a <proc_handle_signals+0x15a>
  103dcd:	25 ff ff fb ff       	and    $0xfffbffff,%eax
  103dd2:	89 47 20             	mov    %eax,0x20(%edi)
  103dd5:	8b 47 70             	mov    0x70(%edi),%eax
  103dd8:	83 f8 01             	cmp    $0x1,%eax
  103ddb:	0f 84 99 00 00 00    	je     103e7a <proc_handle_signals+0x15a>
  103de1:	85 c0                	test   %eax,%eax
  103de3:	74 0f                	je     103df4 <proc_handle_signals+0xd4>
  103de5:	b9 12 00 00 00       	mov    $0x12,%ecx
  103dea:	bb 92 00 00 00       	mov    $0x92,%ebx
  103def:	e9 4c ff ff ff       	jmp    103d40 <proc_handle_signals+0x20>
  103df4:	83 7f 10 05          	cmpl   $0x5,0x10(%edi)
  103df8:	74 65                	je     103e5f <proc_handle_signals+0x13f>
  103dfa:	8b 47 20             	mov    0x20(%edi),%eax
  103dfd:	a9 00 00 08 00       	test   $0x80000,%eax
  103e02:	74 6c                	je     103e70 <proc_handle_signals+0x150>
  103e04:	25 ff ff f7 ff       	and    $0xfff7ffff,%eax
  103e09:	89 47 20             	mov    %eax,0x20(%edi)
  103e0c:	8b 47 74             	mov    0x74(%edi),%eax
  103e0f:	83 f8 01             	cmp    $0x1,%eax
  103e12:	74 5c                	je     103e70 <proc_handle_signals+0x150>
  103e14:	85 c0                	test   %eax,%eax
  103e16:	74 0f                	je     103e27 <proc_handle_signals+0x107>
  103e18:	b9 13 00 00 00       	mov    $0x13,%ecx
  103e1d:	bb 93 00 00 00       	mov    $0x93,%ebx
  103e22:	e9 19 ff ff ff       	jmp    103d40 <proc_handle_signals+0x20>
  103e27:	8b 47 20             	mov    0x20(%edi),%eax
  103e2a:	c7 47 10 05 00 00 00 	movl   $0x5,0x10(%edi)
  103e31:	a9 00 00 10 00       	test   $0x100000,%eax
  103e36:	74 4c                	je     103e84 <proc_handle_signals+0x164>
  103e38:	25 ff ff ef ff       	and    $0xffefffff,%eax
  103e3d:	89 47 20             	mov    %eax,0x20(%edi)
  103e40:	8b 47 78             	mov    0x78(%edi),%eax
  103e43:	83 f8 01             	cmp    $0x1,%eax
  103e46:	74 3c                	je     103e84 <proc_handle_signals+0x164>
  103e48:	85 c0                	test   %eax,%eax
  103e4a:	0f 84 44 ff ff ff    	je     103d94 <proc_handle_signals+0x74>
  103e50:	b9 14 00 00 00       	mov    $0x14,%ecx
  103e55:	bb 94 00 00 00       	mov    $0x94,%ebx
  103e5a:	e9 e1 fe ff ff       	jmp    103d40 <proc_handle_signals+0x20>
  103e5f:	8b 47 20             	mov    0x20(%edi),%eax
  103e62:	c7 47 10 02 00 00 00 	movl   $0x2,0x10(%edi)
  103e69:	a9 00 00 08 00       	test   $0x80000,%eax
  103e6e:	75 94                	jne    103e04 <proc_handle_signals+0xe4>
  103e70:	bb 93 00 00 00       	mov    $0x93,%ebx
  103e75:	e9 cf fe ff ff       	jmp    103d49 <proc_handle_signals+0x29>
  103e7a:	bb 92 00 00 00       	mov    $0x92,%ebx
  103e7f:	e9 c5 fe ff ff       	jmp    103d49 <proc_handle_signals+0x29>
  103e84:	bb 94 00 00 00       	mov    $0x94,%ebx
  103e89:	e9 bb fe ff ff       	jmp    103d49 <proc_handle_signals+0x29>
  103e8e:	66 90                	xchg   %ax,%ax

00103e90 <vma_find>:
  103e90:	8b 44 24 04          	mov    0x4(%esp),%eax
  103e94:	8b 54 24 08          	mov    0x8(%esp),%edx
  103e98:	8b 40 1c             	mov    0x1c(%eax),%eax
  103e9b:	85 c0                	test   %eax,%eax
  103e9d:	74 11                	je     103eb0 <vma_find+0x20>
  103e9f:	90                   	nop
  103ea0:	3b 10                	cmp    (%eax),%edx
  103ea2:	72 05                	jb     103ea9 <vma_find+0x19>
  103ea4:	3b 50 04             	cmp    0x4(%eax),%edx
  103ea7:	72 07                	jb     103eb0 <vma_find+0x20>
  103ea9:	8b 40 10             	mov    0x10(%eax),%eax
  103eac:	85 c0                	test   %eax,%eax
  103eae:	75 f0                	jne    103ea0 <vma_find+0x10>
  103eb0:	c3                   	ret
  103eb1:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  103eb8:	00 
  103eb9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00103ec0 <vma_insert>:
  103ec0:	56                   	push   %esi
  103ec1:	53                   	push   %ebx
  103ec2:	83 ec 10             	sub    $0x10,%esp
  103ec5:	8b 74 24 1c          	mov    0x1c(%esp),%esi
  103ec9:	ff 35 24 20 11 00    	push   0x112024
  103ecf:	e8 0c f9 ff ff       	call   1037e0 <slab_alloc>
  103ed4:	83 c4 10             	add    $0x10,%esp
  103ed7:	89 c1                	mov    %eax,%ecx
  103ed9:	85 c0                	test   %eax,%eax
  103edb:	74 4e                	je     103f2b <vma_insert+0x6b>
  103edd:	8b 54 24 14          	mov    0x14(%esp),%edx
  103ee1:	8d 5e 1c             	lea    0x1c(%esi),%ebx
  103ee4:	81 e2 00 f0 ff ff    	and    $0xfffff000,%edx
  103eea:	89 10                	mov    %edx,(%eax)
  103eec:	8b 44 24 18          	mov    0x18(%esp),%eax
  103ef0:	c7 41 0c 00 00 00 00 	movl   $0x0,0xc(%ecx)
  103ef7:	05 ff 0f 00 00       	add    $0xfff,%eax
  103efc:	25 00 f0 ff ff       	and    $0xfffff000,%eax
  103f01:	89 41 04             	mov    %eax,0x4(%ecx)
  103f04:	8b 44 24 1c          	mov    0x1c(%esp),%eax
  103f08:	89 41 08             	mov    %eax,0x8(%ecx)
  103f0b:	8b 46 1c             	mov    0x1c(%esi),%eax
  103f0e:	85 c0                	test   %eax,%eax
  103f10:	75 10                	jne    103f22 <vma_insert+0x62>
  103f12:	eb 12                	jmp    103f26 <vma_insert+0x66>
  103f14:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  103f18:	8d 58 10             	lea    0x10(%eax),%ebx
  103f1b:	8b 40 10             	mov    0x10(%eax),%eax
  103f1e:	85 c0                	test   %eax,%eax
  103f20:	74 04                	je     103f26 <vma_insert+0x66>
  103f22:	39 10                	cmp    %edx,(%eax)
  103f24:	72 f2                	jb     103f18 <vma_insert+0x58>
  103f26:	89 41 10             	mov    %eax,0x10(%ecx)
  103f29:	89 0b                	mov    %ecx,(%ebx)
  103f2b:	83 c4 04             	add    $0x4,%esp
  103f2e:	89 c8                	mov    %ecx,%eax
  103f30:	5b                   	pop    %ebx
  103f31:	5e                   	pop    %esi
  103f32:	c3                   	ret
  103f33:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  103f3a:	00 
  103f3b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00103f40 <vma_remove>:
  103f40:	8b 54 24 04          	mov    0x4(%esp),%edx
  103f44:	8b 4c 24 08          	mov    0x8(%esp),%ecx
  103f48:	8b 42 1c             	mov    0x1c(%edx),%eax
  103f4b:	85 c0                	test   %eax,%eax
  103f4d:	74 1e                	je     103f6d <vma_remove+0x2d>
  103f4f:	83 c2 1c             	add    $0x1c,%edx
  103f52:	39 c1                	cmp    %eax,%ecx
  103f54:	75 0e                	jne    103f64 <vma_remove+0x24>
  103f56:	eb 1b                	jmp    103f73 <vma_remove+0x33>
  103f58:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  103f5f:	00 
  103f60:	39 c1                	cmp    %eax,%ecx
  103f62:	74 0c                	je     103f70 <vma_remove+0x30>
  103f64:	89 c2                	mov    %eax,%edx
  103f66:	8b 40 10             	mov    0x10(%eax),%eax
  103f69:	85 c0                	test   %eax,%eax
  103f6b:	75 f3                	jne    103f60 <vma_remove+0x20>
  103f6d:	c3                   	ret
  103f6e:	66 90                	xchg   %ax,%ax
  103f70:	83 c2 10             	add    $0x10,%edx
  103f73:	8b 41 10             	mov    0x10(%ecx),%eax
  103f76:	89 02                	mov    %eax,(%edx)
  103f78:	a1 24 20 11 00       	mov    0x112024,%eax
  103f7d:	89 4c 24 08          	mov    %ecx,0x8(%esp)
  103f81:	89 44 24 04          	mov    %eax,0x4(%esp)
  103f85:	e9 d6 f9 ff ff       	jmp    103960 <slab_free>
  103f8a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00103f90 <vma_dump>:
  103f90:	56                   	push   %esi
  103f91:	53                   	push   %ebx
  103f92:	83 ec 08             	sub    $0x8,%esp
  103f95:	8b 5c 24 14          	mov    0x14(%esp),%ebx
  103f99:	ff 33                	push   (%ebx)
  103f9b:	68 6c 93 10 00       	push   $0x10936c
  103fa0:	6a 01                	push   $0x1
  103fa2:	e8 89 0b 00 00       	call   104b30 <klog>
  103fa7:	8b 5b 1c             	mov    0x1c(%ebx),%ebx
  103faa:	83 c4 10             	add    $0x10,%esp
  103fad:	85 db                	test   %ebx,%ebx
  103faf:	74 67                	je     104018 <vma_dump+0x88>
  103fb1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  103fb8:	8b 43 08             	mov    0x8(%ebx),%eax
  103fbb:	89 c2                	mov    %eax,%edx
  103fbd:	83 e2 08             	and    $0x8,%edx
  103fc0:	83 fa 01             	cmp    $0x1,%edx
  103fc3:	89 c2                	mov    %eax,%edx
  103fc5:	19 f6                	sbb    %esi,%esi
  103fc7:	83 e2 04             	and    $0x4,%edx
  103fca:	83 e6 fd             	and    $0xfffffffd,%esi
  103fcd:	83 c6 73             	add    $0x73,%esi
  103fd0:	83 fa 01             	cmp    $0x1,%edx
  103fd3:	89 c2                	mov    %eax,%edx
  103fd5:	19 c9                	sbb    %ecx,%ecx
  103fd7:	83 e2 02             	and    $0x2,%edx
  103fda:	56                   	push   %esi
  103fdb:	83 e1 b5             	and    $0xffffffb5,%ecx
  103fde:	83 c1 78             	add    $0x78,%ecx
  103fe1:	83 fa 01             	cmp    $0x1,%edx
  103fe4:	19 d2                	sbb    %edx,%edx
  103fe6:	83 e0 01             	and    $0x1,%eax
  103fe9:	51                   	push   %ecx
  103fea:	83 e2 b6             	and    $0xffffffb6,%edx
  103fed:	83 c2 77             	add    $0x77,%edx
  103ff0:	83 f8 01             	cmp    $0x1,%eax
  103ff3:	19 c0                	sbb    %eax,%eax
  103ff5:	52                   	push   %edx
  103ff6:	83 e0 bb             	and    $0xffffffbb,%eax
  103ff9:	83 c0 72             	add    $0x72,%eax
  103ffc:	50                   	push   %eax
  103ffd:	ff 73 04             	push   0x4(%ebx)
  104000:	ff 33                	push   (%ebx)
  104002:	68 70 9b 10 00       	push   $0x109b70
  104007:	6a 01                	push   $0x1
  104009:	e8 22 0b 00 00       	call   104b30 <klog>
  10400e:	8b 5b 10             	mov    0x10(%ebx),%ebx
  104011:	83 c4 20             	add    $0x20,%esp
  104014:	85 db                	test   %ebx,%ebx
  104016:	75 a0                	jne    103fb8 <vma_dump+0x28>
  104018:	83 c4 04             	add    $0x4,%esp
  10401b:	5b                   	pop    %ebx
  10401c:	5e                   	pop    %esi
  10401d:	c3                   	ret
  10401e:	66 90                	xchg   %ax,%ax

00104020 <proc_find>:
  104020:	8b 4c 24 04          	mov    0x4(%esp),%ecx
  104024:	b8 60 21 11 00       	mov    $0x112160,%eax
  104029:	31 d2                	xor    %edx,%edx
  10402b:	eb 10                	jmp    10403d <proc_find+0x1d>
  10402d:	8d 76 00             	lea    0x0(%esi),%esi
  104030:	83 c2 01             	add    $0x1,%edx
  104033:	05 64 01 00 00       	add    $0x164,%eax
  104038:	83 fa 40             	cmp    $0x40,%edx
  10403b:	74 1b                	je     104058 <proc_find+0x38>
  10403d:	39 08                	cmp    %ecx,(%eax)
  10403f:	75 ef                	jne    104030 <proc_find+0x10>
  104041:	83 78 10 00          	cmpl   $0x0,0x10(%eax)
  104045:	74 e9                	je     104030 <proc_find+0x10>
  104047:	69 c2 64 01 00 00    	imul   $0x164,%edx,%eax
  10404d:	05 60 21 11 00       	add    $0x112160,%eax
  104052:	c3                   	ret
  104053:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  104058:	31 c0                	xor    %eax,%eax
  10405a:	c3                   	ret
  10405b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00104060 <proc_current>:
  104060:	a1 40 21 11 00       	mov    0x112140,%eax
  104065:	c3                   	ret
  104066:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10406d:	00 
  10406e:	66 90                	xchg   %ax,%ax

00104070 <proc_exit>:
  104070:	a1 40 21 11 00       	mov    0x112140,%eax
  104075:	85 c0                	test   %eax,%eax
  104077:	74 27                	je     1040a0 <proc_exit+0x30>
  104079:	8b 54 24 04          	mov    0x4(%esp),%edx
  10407d:	c7 40 10 04 00 00 00 	movl   $0x4,0x10(%eax)
  104084:	89 50 14             	mov    %edx,0x14(%eax)
  104087:	ff 70 04             	push   0x4(%eax)
  10408a:	e8 91 ff ff ff       	call   104020 <proc_find>
  10408f:	59                   	pop    %ecx
  104090:	85 c0                	test   %eax,%eax
  104092:	74 0c                	je     1040a0 <proc_exit+0x30>
  104094:	6a 11                	push   $0x11
  104096:	50                   	push   %eax
  104097:	e8 24 fc ff ff       	call   103cc0 <proc_signal>
  10409c:	58                   	pop    %eax
  10409d:	5a                   	pop    %edx
  10409e:	66 90                	xchg   %ax,%ax
  1040a0:	fa                   	cli
  1040a1:	f4                   	hlt
  1040a2:	eb fc                	jmp    1040a0 <proc_exit+0x30>
  1040a4:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1040ab:	00 
  1040ac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

001040b0 <proc_fork>:
  1040b0:	57                   	push   %edi
  1040b1:	56                   	push   %esi
  1040b2:	53                   	push   %ebx
  1040b3:	8b 7c 24 10          	mov    0x10(%esp),%edi
  1040b7:	85 ff                	test   %edi,%edi
  1040b9:	0f 84 c1 00 00 00    	je     104180 <proc_fork+0xd0>
  1040bf:	83 ec 0c             	sub    $0xc,%esp
  1040c2:	8d 87 44 01 00 00    	lea    0x144(%edi),%eax
  1040c8:	50                   	push   %eax
  1040c9:	e8 a2 fa ff ff       	call   103b70 <proc_create>
  1040ce:	83 c4 10             	add    $0x10,%esp
  1040d1:	89 c6                	mov    %eax,%esi
  1040d3:	85 c0                	test   %eax,%eax
  1040d5:	0f 84 a5 00 00 00    	je     104180 <proc_fork+0xd0>
  1040db:	8b 47 0c             	mov    0xc(%edi),%eax
  1040de:	8b 0f                	mov    (%edi),%ecx
  1040e0:	83 ec 04             	sub    $0x4,%esp
  1040e3:	8b 57 08             	mov    0x8(%edi),%edx
  1040e6:	89 46 0c             	mov    %eax,0xc(%esi)
  1040e9:	8d 47 28             	lea    0x28(%edi),%eax
  1040ec:	89 4e 04             	mov    %ecx,0x4(%esi)
  1040ef:	89 56 08             	mov    %edx,0x8(%esi)
  1040f2:	68 80 00 00 00       	push   $0x80
  1040f7:	50                   	push   %eax
  1040f8:	8d 46 28             	lea    0x28(%esi),%eax
  1040fb:	50                   	push   %eax
  1040fc:	e8 af d4 ff ff       	call   1015b0 <memcpy>
  104101:	8b 47 24             	mov    0x24(%edi),%eax
  104104:	8b 5f 1c             	mov    0x1c(%edi),%ebx
  104107:	83 c4 10             	add    $0x10,%esp
  10410a:	89 46 24             	mov    %eax,0x24(%esi)
  10410d:	85 db                	test   %ebx,%ebx
  10410f:	74 2d                	je     10413e <proc_fork+0x8e>
  104111:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  104118:	8b 43 08             	mov    0x8(%ebx),%eax
  10411b:	83 e0 fd             	and    $0xfffffffd,%eax
  10411e:	50                   	push   %eax
  10411f:	ff 73 04             	push   0x4(%ebx)
  104122:	ff 33                	push   (%ebx)
  104124:	56                   	push   %esi
  104125:	e8 96 fd ff ff       	call   103ec0 <vma_insert>
  10412a:	83 c4 10             	add    $0x10,%esp
  10412d:	85 c0                	test   %eax,%eax
  10412f:	74 06                	je     104137 <proc_fork+0x87>
  104131:	8b 53 0c             	mov    0xc(%ebx),%edx
  104134:	89 50 0c             	mov    %edx,0xc(%eax)
  104137:	8b 5b 10             	mov    0x10(%ebx),%ebx
  10413a:	85 db                	test   %ebx,%ebx
  10413c:	75 da                	jne    104118 <proc_fork+0x68>
  10413e:	e8 fd ec ff ff       	call   102e40 <pmm_alloc_page>
  104143:	89 c3                	mov    %eax,%ebx
  104145:	85 c0                	test   %eax,%eax
  104147:	74 3f                	je     104188 <proc_fork+0xd8>
  104149:	e8 72 f1 ff ff       	call   1032c0 <paging_get_cr3>
  10414e:	83 ec 04             	sub    $0x4,%esp
  104151:	68 00 10 00 00       	push   $0x1000
  104156:	50                   	push   %eax
  104157:	53                   	push   %ebx
  104158:	e8 53 d4 ff ff       	call   1015b0 <memcpy>
  10415d:	89 5e 18             	mov    %ebx,0x18(%esi)
  104160:	ff 36                	push   (%esi)
  104162:	ff 37                	push   (%edi)
  104164:	68 94 9b 10 00       	push   $0x109b94
  104169:	6a 01                	push   $0x1
  10416b:	e8 c0 09 00 00       	call   104b30 <klog>
  104170:	83 c4 20             	add    $0x20,%esp
  104173:	89 f0                	mov    %esi,%eax
  104175:	5b                   	pop    %ebx
  104176:	5e                   	pop    %esi
  104177:	5f                   	pop    %edi
  104178:	c3                   	ret
  104179:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  104180:	31 f6                	xor    %esi,%esi
  104182:	5b                   	pop    %ebx
  104183:	89 f0                	mov    %esi,%eax
  104185:	5e                   	pop    %esi
  104186:	5f                   	pop    %edi
  104187:	c3                   	ret
  104188:	83 ec 0c             	sub    $0xc,%esp
  10418b:	6a 01                	push   $0x1
  10418d:	e8 de fe ff ff       	call   104070 <proc_exit>
  104192:	66 90                	xchg   %ax,%ax
  104194:	66 90                	xchg   %ax,%ax
  104196:	66 90                	xchg   %ax,%ax
  104198:	66 90                	xchg   %ax,%ax
  10419a:	66 90                	xchg   %ax,%ax
  10419c:	66 90                	xchg   %ax,%ax
  10419e:	66 90                	xchg   %ax,%ax

001041a0 <fb_put_char.part.0>:
  1041a0:	55                   	push   %ebp
  1041a1:	57                   	push   %edi
  1041a2:	56                   	push   %esi
  1041a3:	53                   	push   %ebx
  1041a4:	bb f8 00 00 00       	mov    $0xf8,%ebx
  1041a9:	83 ec 28             	sub    $0x28,%esp
  1041ac:	8b 74 24 3c          	mov    0x3c(%esp),%esi
  1041b0:	8b 7c 24 40          	mov    0x40(%esp),%edi
  1041b4:	89 44 24 10          	mov    %eax,0x10(%esp)
  1041b8:	8d 41 e0             	lea    -0x20(%ecx),%eax
  1041bb:	89 54 24 0c          	mov    %edx,0xc(%esp)
  1041bf:	3c 5f                	cmp    $0x5f,%al
  1041c1:	77 0a                	ja     1041cd <fb_put_char.part.0+0x2d>
  1041c3:	0f be c9             	movsbl %cl,%ecx
  1041c6:	8d 1c cd 00 ff ff ff 	lea    -0x100(,%ecx,8),%ebx
  1041cd:	0f b6 05 60 7a 11 00 	movzbl 0x117a60,%eax
  1041d4:	8b 2d 6c 7a 11 00    	mov    0x117a6c,%ebp
  1041da:	81 c3 a0 a6 10 00    	add    $0x10a6a0,%ebx
  1041e0:	88 44 24 07          	mov    %al,0x7(%esp)
  1041e4:	a1 68 7a 11 00       	mov    0x117a68,%eax
  1041e9:	89 6c 24 20          	mov    %ebp,0x20(%esp)
  1041ed:	89 44 24 14          	mov    %eax,0x14(%esp)
  1041f1:	a1 64 7a 11 00       	mov    0x117a64,%eax
  1041f6:	89 44 24 18          	mov    %eax,0x18(%esp)
  1041fa:	0f b6 05 61 7a 11 00 	movzbl 0x117a61,%eax
  104201:	c0 e8 03             	shr    $0x3,%al
  104204:	0f b6 c0             	movzbl %al,%eax
  104207:	89 c1                	mov    %eax,%ecx
  104209:	89 44 24 08          	mov    %eax,0x8(%esp)
  10420d:	8b 44 24 10          	mov    0x10(%esp),%eax
  104211:	0f af c1             	imul   %ecx,%eax
  104214:	8b 4c 24 0c          	mov    0xc(%esp),%ecx
  104218:	03 05 70 7a 11 00    	add    0x117a70,%eax
  10421e:	89 ca                	mov    %ecx,%edx
  104220:	0f af d5             	imul   %ebp,%edx
  104223:	01 c2                	add    %eax,%edx
  104225:	8d 41 08             	lea    0x8(%ecx),%eax
  104228:	89 44 24 24          	mov    %eax,0x24(%esp)
  10422c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  104230:	89 1c 24             	mov    %ebx,(%esp)
  104233:	89 d0                	mov    %edx,%eax
  104235:	31 c9                	xor    %ecx,%ecx
  104237:	89 54 24 1c          	mov    %edx,0x1c(%esp)
  10423b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  104240:	8b 1c 24             	mov    (%esp),%ebx
  104243:	bd 80 00 00 00       	mov    $0x80,%ebp
  104248:	d3 fd                	sar    %cl,%ebp
  10424a:	0f b6 13             	movzbl (%ebx),%edx
  10424d:	85 ea                	test   %ebp,%edx
  10424f:	89 f5                	mov    %esi,%ebp
  104251:	0f 44 ef             	cmove  %edi,%ebp
  104254:	80 7c 24 07 00       	cmpb   $0x0,0x7(%esp)
  104259:	74 1a                	je     104275 <fb_put_char.part.0+0xd5>
  10425b:	8b 54 24 10          	mov    0x10(%esp),%edx
  10425f:	8b 5c 24 14          	mov    0x14(%esp),%ebx
  104263:	01 ca                	add    %ecx,%edx
  104265:	39 da                	cmp    %ebx,%edx
  104267:	73 0c                	jae    104275 <fb_put_char.part.0+0xd5>
  104269:	8b 54 24 18          	mov    0x18(%esp),%edx
  10426d:	39 54 24 0c          	cmp    %edx,0xc(%esp)
  104271:	73 02                	jae    104275 <fb_put_char.part.0+0xd5>
  104273:	89 28                	mov    %ebp,(%eax)
  104275:	8b 54 24 08          	mov    0x8(%esp),%edx
  104279:	83 c1 01             	add    $0x1,%ecx
  10427c:	01 d0                	add    %edx,%eax
  10427e:	83 f9 08             	cmp    $0x8,%ecx
  104281:	75 bd                	jne    104240 <fb_put_char.part.0+0xa0>
  104283:	8b 4c 24 20          	mov    0x20(%esp),%ecx
  104287:	8b 54 24 1c          	mov    0x1c(%esp),%edx
  10428b:	83 44 24 0c 01       	addl   $0x1,0xc(%esp)
  104290:	8b 1c 24             	mov    (%esp),%ebx
  104293:	01 ca                	add    %ecx,%edx
  104295:	8b 44 24 0c          	mov    0xc(%esp),%eax
  104299:	8b 4c 24 24          	mov    0x24(%esp),%ecx
  10429d:	83 c3 01             	add    $0x1,%ebx
  1042a0:	39 c8                	cmp    %ecx,%eax
  1042a2:	75 8c                	jne    104230 <fb_put_char.part.0+0x90>
  1042a4:	83 c4 28             	add    $0x28,%esp
  1042a7:	5b                   	pop    %ebx
  1042a8:	5e                   	pop    %esi
  1042a9:	5f                   	pop    %edi
  1042aa:	5d                   	pop    %ebp
  1042ab:	c3                   	ret
  1042ac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

001042b0 <vga_init>:
  1042b0:	56                   	push   %esi
  1042b1:	b8 00 80 0b 00       	mov    $0xb8000,%eax
  1042b6:	53                   	push   %ebx
  1042b7:	c6 05 74 7a 11 00 07 	movb   $0x7,0x117a74
  1042be:	66 90                	xchg   %ax,%ax
  1042c0:	ba 20 07 00 00       	mov    $0x720,%edx
  1042c5:	b9 20 07 00 00       	mov    $0x720,%ecx
  1042ca:	83 c0 04             	add    $0x4,%eax
  1042cd:	66 89 50 fc          	mov    %dx,-0x4(%eax)
  1042d1:	66 89 48 fe          	mov    %cx,-0x2(%eax)
  1042d5:	3d a0 8f 0b 00       	cmp    $0xb8fa0,%eax
  1042da:	75 e4                	jne    1042c0 <vga_init+0x10>
  1042dc:	be d4 03 00 00       	mov    $0x3d4,%esi
  1042e1:	c6 05 75 7a 11 00 00 	movb   $0x0,0x117a75
  1042e8:	b8 0e 00 00 00       	mov    $0xe,%eax
  1042ed:	c6 05 76 7a 11 00 00 	movb   $0x0,0x117a76
  1042f4:	89 f2                	mov    %esi,%edx
  1042f6:	ee                   	out    %al,(%dx)
  1042f7:	31 c9                	xor    %ecx,%ecx
  1042f9:	bb d5 03 00 00       	mov    $0x3d5,%ebx
  1042fe:	89 c8                	mov    %ecx,%eax
  104300:	89 da                	mov    %ebx,%edx
  104302:	ee                   	out    %al,(%dx)
  104303:	b8 0f 00 00 00       	mov    $0xf,%eax
  104308:	89 f2                	mov    %esi,%edx
  10430a:	ee                   	out    %al,(%dx)
  10430b:	89 c8                	mov    %ecx,%eax
  10430d:	89 da                	mov    %ebx,%edx
  10430f:	ee                   	out    %al,(%dx)
  104310:	5b                   	pop    %ebx
  104311:	5e                   	pop    %esi
  104312:	c3                   	ret
  104313:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10431a:	00 
  10431b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00104320 <vga_clear>:
  104320:	0f b6 15 74 7a 11 00 	movzbl 0x117a74,%edx
  104327:	56                   	push   %esi
  104328:	b8 00 80 0b 00       	mov    $0xb8000,%eax
  10432d:	53                   	push   %ebx
  10432e:	c1 e2 08             	shl    $0x8,%edx
  104331:	83 ca 20             	or     $0x20,%edx
  104334:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  104338:	66 89 10             	mov    %dx,(%eax)
  10433b:	83 c0 04             	add    $0x4,%eax
  10433e:	66 89 50 fe          	mov    %dx,-0x2(%eax)
  104342:	3d a0 8f 0b 00       	cmp    $0xb8fa0,%eax
  104347:	75 ef                	jne    104338 <vga_clear+0x18>
  104349:	be d4 03 00 00       	mov    $0x3d4,%esi
  10434e:	c6 05 75 7a 11 00 00 	movb   $0x0,0x117a75
  104355:	b8 0e 00 00 00       	mov    $0xe,%eax
  10435a:	c6 05 76 7a 11 00 00 	movb   $0x0,0x117a76
  104361:	89 f2                	mov    %esi,%edx
  104363:	ee                   	out    %al,(%dx)
  104364:	31 c9                	xor    %ecx,%ecx
  104366:	bb d5 03 00 00       	mov    $0x3d5,%ebx
  10436b:	89 c8                	mov    %ecx,%eax
  10436d:	89 da                	mov    %ebx,%edx
  10436f:	ee                   	out    %al,(%dx)
  104370:	b8 0f 00 00 00       	mov    $0xf,%eax
  104375:	89 f2                	mov    %esi,%edx
  104377:	ee                   	out    %al,(%dx)
  104378:	89 c8                	mov    %ecx,%eax
  10437a:	89 da                	mov    %ebx,%edx
  10437c:	ee                   	out    %al,(%dx)
  10437d:	5b                   	pop    %ebx
  10437e:	5e                   	pop    %esi
  10437f:	c3                   	ret

00104380 <vga_set_fg>:
  104380:	0f b6 05 74 7a 11 00 	movzbl 0x117a74,%eax
  104387:	0f b6 54 24 04       	movzbl 0x4(%esp),%edx
  10438c:	83 e0 f0             	and    $0xfffffff0,%eax
  10438f:	83 e2 0f             	and    $0xf,%edx
  104392:	09 d0                	or     %edx,%eax
  104394:	a2 74 7a 11 00       	mov    %al,0x117a74
  104399:	c3                   	ret
  10439a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

001043a0 <vga_set_bg>:
  1043a0:	0f b6 05 74 7a 11 00 	movzbl 0x117a74,%eax
  1043a7:	0f b6 54 24 04       	movzbl 0x4(%esp),%edx
  1043ac:	83 e0 0f             	and    $0xf,%eax
  1043af:	c1 e2 04             	shl    $0x4,%edx
  1043b2:	09 d0                	or     %edx,%eax
  1043b4:	a2 74 7a 11 00       	mov    %al,0x117a74
  1043b9:	c3                   	ret
  1043ba:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

001043c0 <vga_set_color>:
  1043c0:	0f b6 44 24 08       	movzbl 0x8(%esp),%eax
  1043c5:	0f b6 54 24 04       	movzbl 0x4(%esp),%edx
  1043ca:	c1 e0 04             	shl    $0x4,%eax
  1043cd:	83 e2 0f             	and    $0xf,%edx
  1043d0:	09 d0                	or     %edx,%eax
  1043d2:	a2 74 7a 11 00       	mov    %al,0x117a74
  1043d7:	c3                   	ret
  1043d8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1043df:	00 

001043e0 <vga_get_col>:
  1043e0:	0f b6 05 76 7a 11 00 	movzbl 0x117a76,%eax
  1043e7:	c3                   	ret
  1043e8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1043ef:	00 

001043f0 <vga_get_row>:
  1043f0:	0f b6 05 75 7a 11 00 	movzbl 0x117a75,%eax
  1043f7:	c3                   	ret
  1043f8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1043ff:	00 

00104400 <vga_set_cursor>:
  104400:	56                   	push   %esi
  104401:	ba 4f 00 00 00       	mov    $0x4f,%edx
  104406:	be d4 03 00 00       	mov    $0x3d4,%esi
  10440b:	53                   	push   %ebx
  10440c:	8b 44 24 0c          	mov    0xc(%esp),%eax
  104410:	8b 4c 24 10          	mov    0x10(%esp),%ecx
  104414:	38 d0                	cmp    %dl,%al
  104416:	0f 47 c2             	cmova  %edx,%eax
  104419:	ba 18 00 00 00       	mov    $0x18,%edx
  10441e:	38 d1                	cmp    %dl,%cl
  104420:	0f 47 ca             	cmova  %edx,%ecx
  104423:	a2 76 7a 11 00       	mov    %al,0x117a76
  104428:	0f b6 c0             	movzbl %al,%eax
  10442b:	89 f2                	mov    %esi,%edx
  10442d:	88 0d 75 7a 11 00    	mov    %cl,0x117a75
  104433:	0f b6 c9             	movzbl %cl,%ecx
  104436:	8d 0c 89             	lea    (%ecx,%ecx,4),%ecx
  104439:	c1 e1 04             	shl    $0x4,%ecx
  10443c:	01 c1                	add    %eax,%ecx
  10443e:	b8 0e 00 00 00       	mov    $0xe,%eax
  104443:	ee                   	out    %al,(%dx)
  104444:	bb d5 03 00 00       	mov    $0x3d5,%ebx
  104449:	89 c8                	mov    %ecx,%eax
  10444b:	66 c1 e8 08          	shr    $0x8,%ax
  10444f:	89 da                	mov    %ebx,%edx
  104451:	ee                   	out    %al,(%dx)
  104452:	b8 0f 00 00 00       	mov    $0xf,%eax
  104457:	89 f2                	mov    %esi,%edx
  104459:	ee                   	out    %al,(%dx)
  10445a:	89 c8                	mov    %ecx,%eax
  10445c:	89 da                	mov    %ebx,%edx
  10445e:	ee                   	out    %al,(%dx)
  10445f:	5b                   	pop    %ebx
  104460:	5e                   	pop    %esi
  104461:	c3                   	ret
  104462:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  104469:	00 
  10446a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00104470 <vga_putchar>:
  104470:	56                   	push   %esi
  104471:	0f b6 05 75 7a 11 00 	movzbl 0x117a75,%eax
  104478:	53                   	push   %ebx
  104479:	8b 4c 24 0c          	mov    0xc(%esp),%ecx
  10447d:	80 f9 0a             	cmp    $0xa,%cl
  104480:	74 2e                	je     1044b0 <vga_putchar+0x40>
  104482:	0f 8f aa 00 00 00    	jg     104532 <vga_putchar+0xc2>
  104488:	80 f9 08             	cmp    $0x8,%cl
  10448b:	0f 84 2c 01 00 00    	je     1045bd <vga_putchar+0x14d>
  104491:	80 f9 09             	cmp    $0x9,%cl
  104494:	0f 85 e0 00 00 00    	jne    10457a <vga_putchar+0x10a>
  10449a:	0f b6 35 76 7a 11 00 	movzbl 0x117a76,%esi
  1044a1:	8d 5e 04             	lea    0x4(%esi),%ebx
  1044a4:	83 e3 fc             	and    $0xfffffffc,%ebx
  1044a7:	80 fb 4f             	cmp    $0x4f,%bl
  1044aa:	0f 86 02 01 00 00    	jbe    1045b2 <vga_putchar+0x142>
  1044b0:	83 c0 01             	add    $0x1,%eax
  1044b3:	c6 05 76 7a 11 00 00 	movb   $0x0,0x117a76
  1044ba:	31 db                	xor    %ebx,%ebx
  1044bc:	a2 75 7a 11 00       	mov    %al,0x117a75
  1044c1:	3c 18                	cmp    $0x18,%al
  1044c3:	76 7f                	jbe    104544 <vga_putchar+0xd4>
  1044c5:	b9 40 81 0b 00       	mov    $0xb8140,%ecx
  1044ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1044d0:	8d 81 60 ff ff ff    	lea    -0xa0(%ecx),%eax
  1044d6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1044dd:	00 
  1044de:	66 90                	xchg   %ax,%ax
  1044e0:	0f b7 10             	movzwl (%eax),%edx
  1044e3:	83 c0 02             	add    $0x2,%eax
  1044e6:	66 89 90 5e ff ff ff 	mov    %dx,-0xa2(%eax)
  1044ed:	39 c1                	cmp    %eax,%ecx
  1044ef:	75 ef                	jne    1044e0 <vga_putchar+0x70>
  1044f1:	81 c1 a0 00 00 00    	add    $0xa0,%ecx
  1044f7:	81 f9 40 90 0b 00    	cmp    $0xb9040,%ecx
  1044fd:	75 d1                	jne    1044d0 <vga_putchar+0x60>
  1044ff:	b8 00 8f 0b 00       	mov    $0xb8f00,%eax
  104504:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  104508:	ba 20 07 00 00       	mov    $0x720,%edx
  10450d:	b9 20 07 00 00       	mov    $0x720,%ecx
  104512:	83 c0 04             	add    $0x4,%eax
  104515:	66 89 50 fc          	mov    %dx,-0x4(%eax)
  104519:	66 89 48 fe          	mov    %cx,-0x2(%eax)
  10451d:	3d a0 8f 0b 00       	cmp    $0xb8fa0,%eax
  104522:	75 e4                	jne    104508 <vga_putchar+0x98>
  104524:	c6 05 75 7a 11 00 18 	movb   $0x18,0x117a75
  10452b:	b9 80 07 00 00       	mov    $0x780,%ecx
  104530:	eb 1b                	jmp    10454d <vga_putchar+0xdd>
  104532:	80 f9 0d             	cmp    $0xd,%cl
  104535:	75 43                	jne    10457a <vga_putchar+0x10a>
  104537:	c6 05 76 7a 11 00 00 	movb   $0x0,0x117a76
  10453e:	31 db                	xor    %ebx,%ebx
  104540:	3c 18                	cmp    $0x18,%al
  104542:	77 81                	ja     1044c5 <vga_putchar+0x55>
  104544:	0f b6 c8             	movzbl %al,%ecx
  104547:	8d 0c 89             	lea    (%ecx,%ecx,4),%ecx
  10454a:	c1 e1 04             	shl    $0x4,%ecx
  10454d:	be d4 03 00 00       	mov    $0x3d4,%esi
  104552:	01 d9                	add    %ebx,%ecx
  104554:	b8 0e 00 00 00       	mov    $0xe,%eax
  104559:	89 f2                	mov    %esi,%edx
  10455b:	ee                   	out    %al,(%dx)
  10455c:	bb d5 03 00 00       	mov    $0x3d5,%ebx
  104561:	89 c8                	mov    %ecx,%eax
  104563:	66 c1 e8 08          	shr    $0x8,%ax
  104567:	89 da                	mov    %ebx,%edx
  104569:	ee                   	out    %al,(%dx)
  10456a:	b8 0f 00 00 00       	mov    $0xf,%eax
  10456f:	89 f2                	mov    %esi,%edx
  104571:	ee                   	out    %al,(%dx)
  104572:	89 c8                	mov    %ecx,%eax
  104574:	89 da                	mov    %ebx,%edx
  104576:	ee                   	out    %al,(%dx)
  104577:	5b                   	pop    %ebx
  104578:	5e                   	pop    %esi
  104579:	c3                   	ret
  10457a:	0f b6 d0             	movzbl %al,%edx
  10457d:	0f b6 35 76 7a 11 00 	movzbl 0x117a76,%esi
  104584:	0f b6 c9             	movzbl %cl,%ecx
  104587:	8d 14 92             	lea    (%edx,%edx,4),%edx
  10458a:	c1 e2 04             	shl    $0x4,%edx
  10458d:	89 f3                	mov    %esi,%ebx
  10458f:	01 f2                	add    %esi,%edx
  104591:	0f b6 35 74 7a 11 00 	movzbl 0x117a74,%esi
  104598:	83 c3 01             	add    $0x1,%ebx
  10459b:	01 d2                	add    %edx,%edx
  10459d:	c1 e6 08             	shl    $0x8,%esi
  1045a0:	09 f1                	or     %esi,%ecx
  1045a2:	66 89 8a 00 80 0b 00 	mov    %cx,0xb8000(%edx)
  1045a9:	80 fb 4f             	cmp    $0x4f,%bl
  1045ac:	0f 87 fe fe ff ff    	ja     1044b0 <vga_putchar+0x40>
  1045b2:	88 1d 76 7a 11 00    	mov    %bl,0x117a76
  1045b8:	0f b6 db             	movzbl %bl,%ebx
  1045bb:	eb 83                	jmp    104540 <vga_putchar+0xd0>
  1045bd:	0f b6 0d 76 7a 11 00 	movzbl 0x117a76,%ecx
  1045c4:	31 db                	xor    %ebx,%ebx
  1045c6:	84 c9                	test   %cl,%cl
  1045c8:	0f 84 72 ff ff ff    	je     104540 <vga_putchar+0xd0>
  1045ce:	0f b6 d0             	movzbl %al,%edx
  1045d1:	83 e9 01             	sub    $0x1,%ecx
  1045d4:	8d 14 92             	lea    (%edx,%edx,4),%edx
  1045d7:	0f b6 d9             	movzbl %cl,%ebx
  1045da:	88 0d 76 7a 11 00    	mov    %cl,0x117a76
  1045e0:	c1 e2 04             	shl    $0x4,%edx
  1045e3:	01 da                	add    %ebx,%edx
  1045e5:	0f b6 1d 74 7a 11 00 	movzbl 0x117a74,%ebx
  1045ec:	01 d2                	add    %edx,%edx
  1045ee:	c1 e3 08             	shl    $0x8,%ebx
  1045f1:	83 cb 20             	or     $0x20,%ebx
  1045f4:	66 89 9a 00 80 0b 00 	mov    %bx,0xb8000(%edx)
  1045fb:	0f b6 d9             	movzbl %cl,%ebx
  1045fe:	e9 3d ff ff ff       	jmp    104540 <vga_putchar+0xd0>
  104603:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10460a:	00 
  10460b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00104610 <vga_puts>:
  104610:	53                   	push   %ebx
  104611:	8b 5c 24 08          	mov    0x8(%esp),%ebx
  104615:	0f be 03             	movsbl (%ebx),%eax
  104618:	84 c0                	test   %al,%al
  10461a:	74 15                	je     104631 <vga_puts+0x21>
  10461c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  104620:	50                   	push   %eax
  104621:	83 c3 01             	add    $0x1,%ebx
  104624:	e8 47 fe ff ff       	call   104470 <vga_putchar>
  104629:	0f be 03             	movsbl (%ebx),%eax
  10462c:	5a                   	pop    %edx
  10462d:	84 c0                	test   %al,%al
  10462f:	75 ef                	jne    104620 <vga_puts+0x10>
  104631:	5b                   	pop    %ebx
  104632:	c3                   	ret
  104633:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10463a:	00 
  10463b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00104640 <vga_printf>:
  104640:	53                   	push   %ebx
  104641:	81 ec 08 02 00 00    	sub    $0x208,%esp
  104647:	8d 84 24 14 02 00 00 	lea    0x214(%esp),%eax
  10464e:	50                   	push   %eax
  10464f:	ff b4 24 14 02 00 00 	push   0x214(%esp)
  104656:	68 00 02 00 00       	push   $0x200
  10465b:	8d 5c 24 0c          	lea    0xc(%esp),%ebx
  10465f:	53                   	push   %ebx
  104660:	e8 3b d3 ff ff       	call   1019a0 <vsnprintf>
  104665:	0f be 44 24 10       	movsbl 0x10(%esp),%eax
  10466a:	89 dc                	mov    %ebx,%esp
  10466c:	84 c0                	test   %al,%al
  10466e:	74 16                	je     104686 <vga_printf+0x46>
  104670:	83 ec 0c             	sub    $0xc,%esp
  104673:	83 c3 01             	add    $0x1,%ebx
  104676:	50                   	push   %eax
  104677:	e8 f4 fd ff ff       	call   104470 <vga_putchar>
  10467c:	0f be 03             	movsbl (%ebx),%eax
  10467f:	83 c4 10             	add    $0x10,%esp
  104682:	84 c0                	test   %al,%al
  104684:	75 ea                	jne    104670 <vga_printf+0x30>
  104686:	81 c4 08 02 00 00    	add    $0x208,%esp
  10468c:	5b                   	pop    %ebx
  10468d:	c3                   	ret
  10468e:	66 90                	xchg   %ax,%ax

00104690 <fb_init>:
  104690:	8b 44 24 04          	mov    0x4(%esp),%eax
  104694:	31 d2                	xor    %edx,%edx
  104696:	f7 00 00 10 00 00    	testl  $0x1000,(%eax)
  10469c:	74 06                	je     1046a4 <fb_init+0x14>
  10469e:	80 78 6d 02          	cmpb   $0x2,0x6d(%eax)
  1046a2:	74 0c                	je     1046b0 <fb_init+0x20>
  1046a4:	89 d0                	mov    %edx,%eax
  1046a6:	c3                   	ret
  1046a7:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1046ae:	00 
  1046af:	90                   	nop
  1046b0:	80 78 6c 20          	cmpb   $0x20,0x6c(%eax)
  1046b4:	75 ee                	jne    1046a4 <fb_init+0x14>
  1046b6:	8b 50 58             	mov    0x58(%eax),%edx
  1046b9:	c6 05 61 7a 11 00 20 	movb   $0x20,0x117a61
  1046c0:	c6 05 60 7a 11 00 01 	movb   $0x1,0x117a60
  1046c7:	89 15 70 7a 11 00    	mov    %edx,0x117a70
  1046cd:	8b 50 60             	mov    0x60(%eax),%edx
  1046d0:	89 15 6c 7a 11 00    	mov    %edx,0x117a6c
  1046d6:	8b 50 64             	mov    0x64(%eax),%edx
  1046d9:	8b 40 68             	mov    0x68(%eax),%eax
  1046dc:	89 15 68 7a 11 00    	mov    %edx,0x117a68
  1046e2:	ba 01 00 00 00       	mov    $0x1,%edx
  1046e7:	a3 64 7a 11 00       	mov    %eax,0x117a64
  1046ec:	89 d0                	mov    %edx,%eax
  1046ee:	c3                   	ret
  1046ef:	90                   	nop

001046f0 <fb_put_pixel>:
  1046f0:	80 3d 60 7a 11 00 00 	cmpb   $0x0,0x117a60
  1046f7:	74 41                	je     10473a <fb_put_pixel+0x4a>
  1046f9:	a1 68 7a 11 00       	mov    0x117a68,%eax
  1046fe:	39 44 24 04          	cmp    %eax,0x4(%esp)
  104702:	73 36                	jae    10473a <fb_put_pixel+0x4a>
  104704:	a1 64 7a 11 00       	mov    0x117a64,%eax
  104709:	39 44 24 08          	cmp    %eax,0x8(%esp)
  10470d:	73 2b                	jae    10473a <fb_put_pixel+0x4a>
  10470f:	0f b6 15 61 7a 11 00 	movzbl 0x117a61,%edx
  104716:	8b 44 24 08          	mov    0x8(%esp),%eax
  10471a:	0f af 05 6c 7a 11 00 	imul   0x117a6c,%eax
  104721:	c0 ea 03             	shr    $0x3,%dl
  104724:	0f b6 d2             	movzbl %dl,%edx
  104727:	0f af 54 24 04       	imul   0x4(%esp),%edx
  10472c:	01 d0                	add    %edx,%eax
  10472e:	8b 54 24 0c          	mov    0xc(%esp),%edx
  104732:	03 05 70 7a 11 00    	add    0x117a70,%eax
  104738:	89 10                	mov    %edx,(%eax)
  10473a:	c3                   	ret
  10473b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00104740 <fb_clear>:
  104740:	55                   	push   %ebp
  104741:	57                   	push   %edi
  104742:	56                   	push   %esi
  104743:	53                   	push   %ebx
  104744:	83 ec 08             	sub    $0x8,%esp
  104747:	80 3d 60 7a 11 00 00 	cmpb   $0x0,0x117a60
  10474e:	8b 74 24 1c          	mov    0x1c(%esp),%esi
  104752:	74 5f                	je     1047b3 <fb_clear+0x73>
  104754:	a1 64 7a 11 00       	mov    0x117a64,%eax
  104759:	89 04 24             	mov    %eax,(%esp)
  10475c:	85 c0                	test   %eax,%eax
  10475e:	74 53                	je     1047b3 <fb_clear+0x73>
  104760:	0f b6 0d 61 7a 11 00 	movzbl 0x117a61,%ecx
  104767:	a1 6c 7a 11 00       	mov    0x117a6c,%eax
  10476c:	8b 1d 68 7a 11 00    	mov    0x117a68,%ebx
  104772:	8b 3d 70 7a 11 00    	mov    0x117a70,%edi
  104778:	c0 e9 03             	shr    $0x3,%cl
  10477b:	89 44 24 04          	mov    %eax,0x4(%esp)
  10477f:	0f b6 c9             	movzbl %cl,%ecx
  104782:	85 db                	test   %ebx,%ebx
  104784:	74 2d                	je     1047b3 <fb_clear+0x73>
  104786:	31 ed                	xor    %ebp,%ebp
  104788:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10478f:	00 
  104790:	89 fa                	mov    %edi,%edx
  104792:	31 c0                	xor    %eax,%eax
  104794:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  104798:	83 c0 01             	add    $0x1,%eax
  10479b:	89 32                	mov    %esi,(%edx)
  10479d:	01 ca                	add    %ecx,%edx
  10479f:	39 d8                	cmp    %ebx,%eax
  1047a1:	75 f5                	jne    104798 <fb_clear+0x58>
  1047a3:	8b 44 24 04          	mov    0x4(%esp),%eax
  1047a7:	83 c5 01             	add    $0x1,%ebp
  1047aa:	01 c7                	add    %eax,%edi
  1047ac:	8b 04 24             	mov    (%esp),%eax
  1047af:	39 c5                	cmp    %eax,%ebp
  1047b1:	75 dd                	jne    104790 <fb_clear+0x50>
  1047b3:	83 c4 08             	add    $0x8,%esp
  1047b6:	5b                   	pop    %ebx
  1047b7:	5e                   	pop    %esi
  1047b8:	5f                   	pop    %edi
  1047b9:	5d                   	pop    %ebp
  1047ba:	c3                   	ret
  1047bb:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

001047c0 <fb_fill_rect>:
  1047c0:	55                   	push   %ebp
  1047c1:	57                   	push   %edi
  1047c2:	56                   	push   %esi
  1047c3:	53                   	push   %ebx
  1047c4:	83 ec 10             	sub    $0x10,%esp
  1047c7:	8b 6c 24 28          	mov    0x28(%esp),%ebp
  1047cb:	8b 44 24 30          	mov    0x30(%esp),%eax
  1047cf:	01 e8                	add    %ebp,%eax
  1047d1:	89 44 24 08          	mov    %eax,0x8(%esp)
  1047d5:	39 c5                	cmp    %eax,%ebp
  1047d7:	0f 83 9e 00 00 00    	jae    10487b <fb_fill_rect+0xbb>
  1047dd:	a1 64 7a 11 00       	mov    0x117a64,%eax
  1047e2:	8b 0d 6c 7a 11 00    	mov    0x117a6c,%ecx
  1047e8:	0f b6 1d 61 7a 11 00 	movzbl 0x117a61,%ebx
  1047ef:	0f b6 3d 60 7a 11 00 	movzbl 0x117a60,%edi
  1047f6:	89 44 24 04          	mov    %eax,0x4(%esp)
  1047fa:	8b 44 24 24          	mov    0x24(%esp),%eax
  1047fe:	89 4c 24 0c          	mov    %ecx,0xc(%esp)
  104802:	c0 eb 03             	shr    $0x3,%bl
  104805:	0f af cd             	imul   %ebp,%ecx
  104808:	8b 35 68 7a 11 00    	mov    0x117a68,%esi
  10480e:	0f b6 db             	movzbl %bl,%ebx
  104811:	0f af c3             	imul   %ebx,%eax
  104814:	03 05 70 7a 11 00    	add    0x117a70,%eax
  10481a:	01 c8                	add    %ecx,%eax
  10481c:	8b 4c 24 24          	mov    0x24(%esp),%ecx
  104820:	03 4c 24 2c          	add    0x2c(%esp),%ecx
  104824:	89 04 24             	mov    %eax,(%esp)
  104827:	89 d8                	mov    %ebx,%eax
  104829:	89 fb                	mov    %edi,%ebx
  10482b:	89 c7                	mov    %eax,%edi
  10482d:	8d 76 00             	lea    0x0(%esi),%esi
  104830:	8b 44 24 04          	mov    0x4(%esp),%eax
  104834:	39 c5                	cmp    %eax,%ebp
  104836:	73 43                	jae    10487b <fb_fill_rect+0xbb>
  104838:	8b 44 24 24          	mov    0x24(%esp),%eax
  10483c:	8b 14 24             	mov    (%esp),%edx
  10483f:	39 c8                	cmp    %ecx,%eax
  104841:	73 28                	jae    10486b <fb_fill_rect+0xab>
  104843:	89 6c 24 28          	mov    %ebp,0x28(%esp)
  104847:	eb 1a                	jmp    104863 <fb_fill_rect+0xa3>
  104849:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  104850:	84 db                	test   %bl,%bl
  104852:	74 06                	je     10485a <fb_fill_rect+0x9a>
  104854:	8b 6c 24 34          	mov    0x34(%esp),%ebp
  104858:	89 2a                	mov    %ebp,(%edx)
  10485a:	83 c0 01             	add    $0x1,%eax
  10485d:	01 fa                	add    %edi,%edx
  10485f:	39 c8                	cmp    %ecx,%eax
  104861:	74 04                	je     104867 <fb_fill_rect+0xa7>
  104863:	39 f0                	cmp    %esi,%eax
  104865:	72 e9                	jb     104850 <fb_fill_rect+0x90>
  104867:	8b 6c 24 28          	mov    0x28(%esp),%ebp
  10486b:	8b 54 24 0c          	mov    0xc(%esp),%edx
  10486f:	83 c5 01             	add    $0x1,%ebp
  104872:	01 14 24             	add    %edx,(%esp)
  104875:	39 6c 24 08          	cmp    %ebp,0x8(%esp)
  104879:	75 b5                	jne    104830 <fb_fill_rect+0x70>
  10487b:	83 c4 10             	add    $0x10,%esp
  10487e:	5b                   	pop    %ebx
  10487f:	5e                   	pop    %esi
  104880:	5f                   	pop    %edi
  104881:	5d                   	pop    %ebp
  104882:	c3                   	ret
  104883:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10488a:	00 
  10488b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00104890 <fb_put_char>:
  104890:	80 3d 60 7a 11 00 00 	cmpb   $0x0,0x117a60
  104897:	57                   	push   %edi
  104898:	56                   	push   %esi
  104899:	53                   	push   %ebx
  10489a:	8b 5c 24 10          	mov    0x10(%esp),%ebx
  10489e:	8b 54 24 14          	mov    0x14(%esp),%edx
  1048a2:	8b 4c 24 18          	mov    0x18(%esp),%ecx
  1048a6:	8b 74 24 1c          	mov    0x1c(%esp),%esi
  1048aa:	8b 7c 24 20          	mov    0x20(%esp),%edi
  1048ae:	74 15                	je     1048c5 <fb_put_char+0x35>
  1048b0:	89 7c 24 14          	mov    %edi,0x14(%esp)
  1048b4:	89 d8                	mov    %ebx,%eax
  1048b6:	0f be c9             	movsbl %cl,%ecx
  1048b9:	89 74 24 10          	mov    %esi,0x10(%esp)
  1048bd:	5b                   	pop    %ebx
  1048be:	5e                   	pop    %esi
  1048bf:	5f                   	pop    %edi
  1048c0:	e9 db f8 ff ff       	jmp    1041a0 <fb_put_char.part.0>
  1048c5:	5b                   	pop    %ebx
  1048c6:	5e                   	pop    %esi
  1048c7:	5f                   	pop    %edi
  1048c8:	c3                   	ret
  1048c9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

001048d0 <fb_puts>:
  1048d0:	55                   	push   %ebp
  1048d1:	57                   	push   %edi
  1048d2:	56                   	push   %esi
  1048d3:	53                   	push   %ebx
  1048d4:	8b 5c 24 1c          	mov    0x1c(%esp),%ebx
  1048d8:	8b 6c 24 24          	mov    0x24(%esp),%ebp
  1048dc:	0f be 0b             	movsbl (%ebx),%ecx
  1048df:	84 c9                	test   %cl,%cl
  1048e1:	74 4b                	je     10492e <fb_puts+0x5e>
  1048e3:	8b 7c 24 14          	mov    0x14(%esp),%edi
  1048e7:	8b 74 24 18          	mov    0x18(%esp),%esi
  1048eb:	eb 2a                	jmp    104917 <fb_puts+0x47>
  1048ed:	8d 76 00             	lea    0x0(%esi),%esi
  1048f0:	80 3d 60 7a 11 00 00 	cmpb   $0x0,0x117a60
  1048f7:	74 10                	je     104909 <fb_puts+0x39>
  1048f9:	55                   	push   %ebp
  1048fa:	89 f2                	mov    %esi,%edx
  1048fc:	89 f8                	mov    %edi,%eax
  1048fe:	ff 74 24 24          	push   0x24(%esp)
  104902:	e8 99 f8 ff ff       	call   1041a0 <fb_put_char.part.0>
  104907:	58                   	pop    %eax
  104908:	5a                   	pop    %edx
  104909:	0f be 4b 01          	movsbl 0x1(%ebx),%ecx
  10490d:	83 c3 01             	add    $0x1,%ebx
  104910:	83 c7 08             	add    $0x8,%edi
  104913:	84 c9                	test   %cl,%cl
  104915:	74 17                	je     10492e <fb_puts+0x5e>
  104917:	80 f9 0a             	cmp    $0xa,%cl
  10491a:	75 d4                	jne    1048f0 <fb_puts+0x20>
  10491c:	0f be 4b 01          	movsbl 0x1(%ebx),%ecx
  104920:	83 c3 01             	add    $0x1,%ebx
  104923:	8b 7c 24 14          	mov    0x14(%esp),%edi
  104927:	83 c6 08             	add    $0x8,%esi
  10492a:	84 c9                	test   %cl,%cl
  10492c:	75 e9                	jne    104917 <fb_puts+0x47>
  10492e:	5b                   	pop    %ebx
  10492f:	5e                   	pop    %esi
  104930:	5f                   	pop    %edi
  104931:	5d                   	pop    %ebp
  104932:	c3                   	ret
  104933:	66 90                	xchg   %ax,%ax
  104935:	66 90                	xchg   %ax,%ax
  104937:	66 90                	xchg   %ax,%ax
  104939:	66 90                	xchg   %ax,%ax
  10493b:	66 90                	xchg   %ax,%ax
  10493d:	66 90                	xchg   %ax,%ax
  10493f:	90                   	nop

00104940 <serial_init>:
  104940:	57                   	push   %edi
  104941:	bf f9 03 00 00       	mov    $0x3f9,%edi
  104946:	56                   	push   %esi
  104947:	31 f6                	xor    %esi,%esi
  104949:	89 fa                	mov    %edi,%edx
  10494b:	53                   	push   %ebx
  10494c:	89 f0                	mov    %esi,%eax
  10494e:	ee                   	out    %al,(%dx)
  10494f:	bb fb 03 00 00       	mov    $0x3fb,%ebx
  104954:	b8 80 ff ff ff       	mov    $0xffffff80,%eax
  104959:	89 da                	mov    %ebx,%edx
  10495b:	ee                   	out    %al,(%dx)
  10495c:	b9 f8 03 00 00       	mov    $0x3f8,%ecx
  104961:	b8 01 00 00 00       	mov    $0x1,%eax
  104966:	89 ca                	mov    %ecx,%edx
  104968:	ee                   	out    %al,(%dx)
  104969:	89 f0                	mov    %esi,%eax
  10496b:	89 fa                	mov    %edi,%edx
  10496d:	ee                   	out    %al,(%dx)
  10496e:	b8 03 00 00 00       	mov    $0x3,%eax
  104973:	89 da                	mov    %ebx,%edx
  104975:	ee                   	out    %al,(%dx)
  104976:	b8 c7 ff ff ff       	mov    $0xffffffc7,%eax
  10497b:	ba fa 03 00 00       	mov    $0x3fa,%edx
  104980:	ee                   	out    %al,(%dx)
  104981:	bb fc 03 00 00       	mov    $0x3fc,%ebx
  104986:	b8 0b 00 00 00       	mov    $0xb,%eax
  10498b:	89 da                	mov    %ebx,%edx
  10498d:	ee                   	out    %al,(%dx)
  10498e:	b8 1e 00 00 00       	mov    $0x1e,%eax
  104993:	ee                   	out    %al,(%dx)
  104994:	b8 ae ff ff ff       	mov    $0xffffffae,%eax
  104999:	89 ca                	mov    %ecx,%edx
  10499b:	ee                   	out    %al,(%dx)
  10499c:	ec                   	in     (%dx),%al
  10499d:	31 d2                	xor    %edx,%edx
  10499f:	3c ae                	cmp    $0xae,%al
  1049a1:	75 0d                	jne    1049b0 <serial_init+0x70>
  1049a3:	b8 0f 00 00 00       	mov    $0xf,%eax
  1049a8:	89 da                	mov    %ebx,%edx
  1049aa:	ee                   	out    %al,(%dx)
  1049ab:	ba 01 00 00 00       	mov    $0x1,%edx
  1049b0:	5b                   	pop    %ebx
  1049b1:	89 d0                	mov    %edx,%eax
  1049b3:	5e                   	pop    %esi
  1049b4:	88 15 77 7a 11 00    	mov    %dl,0x117a77
  1049ba:	5f                   	pop    %edi
  1049bb:	c3                   	ret
  1049bc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

001049c0 <serial_putc>:
  1049c0:	80 3d 77 7a 11 00 00 	cmpb   $0x0,0x117a77
  1049c7:	0f b6 4c 24 04       	movzbl 0x4(%esp),%ecx
  1049cc:	74 21                	je     1049ef <serial_putc+0x2f>
  1049ce:	ba fd 03 00 00       	mov    $0x3fd,%edx
  1049d3:	ec                   	in     (%dx),%al
  1049d4:	a8 20                	test   $0x20,%al
  1049d6:	75 0f                	jne    1049e7 <serial_putc+0x27>
  1049d8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1049df:	00 
  1049e0:	f3 90                	pause
  1049e2:	ec                   	in     (%dx),%al
  1049e3:	a8 20                	test   $0x20,%al
  1049e5:	74 f9                	je     1049e0 <serial_putc+0x20>
  1049e7:	ba f8 03 00 00       	mov    $0x3f8,%edx
  1049ec:	89 c8                	mov    %ecx,%eax
  1049ee:	ee                   	out    %al,(%dx)
  1049ef:	c3                   	ret

001049f0 <serial_puts>:
  1049f0:	55                   	push   %ebp
  1049f1:	57                   	push   %edi
  1049f2:	56                   	push   %esi
  1049f3:	53                   	push   %ebx
  1049f4:	8b 5c 24 14          	mov    0x14(%esp),%ebx
  1049f8:	0f b6 03             	movzbl (%ebx),%eax
  1049fb:	84 c0                	test   %al,%al
  1049fd:	74 46                	je     104a45 <serial_puts+0x55>
  1049ff:	b9 fd 03 00 00       	mov    $0x3fd,%ecx
  104a04:	be f8 03 00 00       	mov    $0x3f8,%esi
  104a09:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  104a10:	0f b6 2d 77 7a 11 00 	movzbl 0x117a77,%ebp
  104a17:	83 c3 01             	add    $0x1,%ebx
  104a1a:	3c 0a                	cmp    $0xa,%al
  104a1c:	74 32                	je     104a50 <serial_puts+0x60>
  104a1e:	89 e8                	mov    %ebp,%eax
  104a20:	0f b6 7b ff          	movzbl -0x1(%ebx),%edi
  104a24:	84 c0                	test   %al,%al
  104a26:	75 0a                	jne    104a32 <serial_puts+0x42>
  104a28:	eb 14                	jmp    104a3e <serial_puts+0x4e>
  104a2a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  104a30:	f3 90                	pause
  104a32:	89 ca                	mov    %ecx,%edx
  104a34:	ec                   	in     (%dx),%al
  104a35:	a8 20                	test   $0x20,%al
  104a37:	74 f7                	je     104a30 <serial_puts+0x40>
  104a39:	89 f8                	mov    %edi,%eax
  104a3b:	89 f2                	mov    %esi,%edx
  104a3d:	ee                   	out    %al,(%dx)
  104a3e:	0f b6 03             	movzbl (%ebx),%eax
  104a41:	84 c0                	test   %al,%al
  104a43:	75 cb                	jne    104a10 <serial_puts+0x20>
  104a45:	5b                   	pop    %ebx
  104a46:	5e                   	pop    %esi
  104a47:	5f                   	pop    %edi
  104a48:	5d                   	pop    %ebp
  104a49:	c3                   	ret
  104a4a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  104a50:	89 e8                	mov    %ebp,%eax
  104a52:	84 c0                	test   %al,%al
  104a54:	74 e8                	je     104a3e <serial_puts+0x4e>
  104a56:	89 ca                	mov    %ecx,%edx
  104a58:	ec                   	in     (%dx),%al
  104a59:	a8 20                	test   $0x20,%al
  104a5b:	75 1d                	jne    104a7a <serial_puts+0x8a>
  104a5d:	8d 76 00             	lea    0x0(%esi),%esi
  104a60:	f3 90                	pause
  104a62:	89 ca                	mov    %ecx,%edx
  104a64:	ec                   	in     (%dx),%al
  104a65:	a8 20                	test   $0x20,%al
  104a67:	74 f7                	je     104a60 <serial_puts+0x70>
  104a69:	0f b6 2d 77 7a 11 00 	movzbl 0x117a77,%ebp
  104a70:	b8 0d 00 00 00       	mov    $0xd,%eax
  104a75:	89 f2                	mov    %esi,%edx
  104a77:	ee                   	out    %al,(%dx)
  104a78:	eb a4                	jmp    104a1e <serial_puts+0x2e>
  104a7a:	b8 0d 00 00 00       	mov    $0xd,%eax
  104a7f:	ba f8 03 00 00       	mov    $0x3f8,%edx
  104a84:	ee                   	out    %al,(%dx)
  104a85:	0f b6 7b ff          	movzbl -0x1(%ebx),%edi
  104a89:	eb a7                	jmp    104a32 <serial_puts+0x42>
  104a8b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00104a90 <serial_printf>:
  104a90:	53                   	push   %ebx
  104a91:	81 ec 08 02 00 00    	sub    $0x208,%esp
  104a97:	8d 84 24 14 02 00 00 	lea    0x214(%esp),%eax
  104a9e:	50                   	push   %eax
  104a9f:	ff b4 24 14 02 00 00 	push   0x214(%esp)
  104aa6:	68 00 02 00 00       	push   $0x200
  104aab:	8d 5c 24 0c          	lea    0xc(%esp),%ebx
  104aaf:	53                   	push   %ebx
  104ab0:	e8 eb ce ff ff       	call   1019a0 <vsnprintf>
  104ab5:	89 1c 24             	mov    %ebx,(%esp)
  104ab8:	e8 33 ff ff ff       	call   1049f0 <serial_puts>
  104abd:	81 c4 18 02 00 00    	add    $0x218,%esp
  104ac3:	5b                   	pop    %ebx
  104ac4:	c3                   	ret
  104ac5:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  104acc:	00 
  104acd:	8d 76 00             	lea    0x0(%esi),%esi

00104ad0 <serial_has_data>:
  104ad0:	0f b6 05 77 7a 11 00 	movzbl 0x117a77,%eax
  104ad7:	84 c0                	test   %al,%al
  104ad9:	74 09                	je     104ae4 <serial_has_data+0x14>
  104adb:	ba fd 03 00 00       	mov    $0x3fd,%edx
  104ae0:	ec                   	in     (%dx),%al
  104ae1:	83 e0 01             	and    $0x1,%eax
  104ae4:	c3                   	ret
  104ae5:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  104aec:	00 
  104aed:	8d 76 00             	lea    0x0(%esi),%esi

00104af0 <serial_getc>:
  104af0:	31 c0                	xor    %eax,%eax
  104af2:	80 3d 77 7a 11 00 00 	cmpb   $0x0,0x117a77
  104af9:	74 25                	je     104b20 <serial_getc+0x30>
  104afb:	ba fd 03 00 00       	mov    $0x3fd,%edx
  104b00:	ec                   	in     (%dx),%al
  104b01:	a8 01                	test   $0x1,%al
  104b03:	74 0b                	je     104b10 <serial_getc+0x20>
  104b05:	ba f8 03 00 00       	mov    $0x3f8,%edx
  104b0a:	ec                   	in     (%dx),%al
  104b0b:	c3                   	ret
  104b0c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  104b10:	f3 90                	pause
  104b12:	80 3d 77 7a 11 00 00 	cmpb   $0x0,0x117a77
  104b19:	74 f5                	je     104b10 <serial_getc+0x20>
  104b1b:	eb e3                	jmp    104b00 <serial_getc+0x10>
  104b1d:	8d 76 00             	lea    0x0(%esi),%esi
  104b20:	c3                   	ret
  104b21:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  104b28:	00 
  104b29:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00104b30 <klog>:
  104b30:	56                   	push   %esi
  104b31:	53                   	push   %ebx
  104b32:	81 ec 04 02 00 00    	sub    $0x204,%esp
  104b38:	8d 84 24 18 02 00 00 	lea    0x218(%esp),%eax
  104b3f:	8b b4 24 10 02 00 00 	mov    0x210(%esp),%esi
  104b46:	50                   	push   %eax
  104b47:	ff b4 24 18 02 00 00 	push   0x218(%esp)
  104b4e:	68 00 02 00 00       	push   $0x200
  104b53:	8d 5c 24 0c          	lea    0xc(%esp),%ebx
  104b57:	53                   	push   %ebx
  104b58:	e8 43 ce ff ff       	call   1019a0 <vsnprintf>
  104b5d:	89 dc                	mov    %ebx,%esp
  104b5f:	b8 84 93 10 00       	mov    $0x109384,%eax
  104b64:	83 fe 04             	cmp    $0x4,%esi
  104b67:	77 07                	ja     104b70 <klog+0x40>
  104b69:	8b 04 b5 a0 a9 10 00 	mov    0x10a9a0(,%esi,4),%eax
  104b70:	83 ec 04             	sub    $0x4,%esp
  104b73:	53                   	push   %ebx
  104b74:	50                   	push   %eax
  104b75:	68 88 93 10 00       	push   $0x109388
  104b7a:	e8 11 ff ff ff       	call   104a90 <serial_printf>
  104b7f:	81 c4 14 02 00 00    	add    $0x214,%esp
  104b85:	5b                   	pop    %ebx
  104b86:	5e                   	pop    %esi
  104b87:	c3                   	ret
  104b88:	66 90                	xchg   %ax,%ax
  104b8a:	66 90                	xchg   %ax,%ax
  104b8c:	66 90                	xchg   %ax,%ax
  104b8e:	66 90                	xchg   %ax,%ax

00104b90 <pci_read32>:
  104b90:	0f b6 44 24 04       	movzbl 0x4(%esp),%eax
  104b95:	8b 54 24 10          	mov    0x10(%esp),%edx
  104b99:	c1 e0 10             	shl    $0x10,%eax
  104b9c:	81 e2 fc 00 00 00    	and    $0xfc,%edx
  104ba2:	09 d0                	or     %edx,%eax
  104ba4:	0f b6 54 24 0c       	movzbl 0xc(%esp),%edx
  104ba9:	c1 e2 08             	shl    $0x8,%edx
  104bac:	09 d0                	or     %edx,%eax
  104bae:	0f b6 54 24 08       	movzbl 0x8(%esp),%edx
  104bb3:	c1 e2 0b             	shl    $0xb,%edx
  104bb6:	09 d0                	or     %edx,%eax
  104bb8:	ba f8 0c 00 00       	mov    $0xcf8,%edx
  104bbd:	0d 00 00 00 80       	or     $0x80000000,%eax
  104bc2:	ef                   	out    %eax,(%dx)
  104bc3:	ba fc 0c 00 00       	mov    $0xcfc,%edx
  104bc8:	ed                   	in     (%dx),%eax
  104bc9:	c3                   	ret
  104bca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00104bd0 <pci_read16>:
  104bd0:	8b 4c 24 10          	mov    0x10(%esp),%ecx
  104bd4:	0f b6 44 24 04       	movzbl 0x4(%esp),%eax
  104bd9:	89 ca                	mov    %ecx,%edx
  104bdb:	c1 e0 10             	shl    $0x10,%eax
  104bde:	81 e2 fc 00 00 00    	and    $0xfc,%edx
  104be4:	09 d0                	or     %edx,%eax
  104be6:	0f b6 54 24 08       	movzbl 0x8(%esp),%edx
  104beb:	c1 e2 0b             	shl    $0xb,%edx
  104bee:	09 d0                	or     %edx,%eax
  104bf0:	0f b6 54 24 0c       	movzbl 0xc(%esp),%edx
  104bf5:	c1 e2 08             	shl    $0x8,%edx
  104bf8:	09 d0                	or     %edx,%eax
  104bfa:	ba f8 0c 00 00       	mov    $0xcf8,%edx
  104bff:	0d 00 00 00 80       	or     $0x80000000,%eax
  104c04:	ef                   	out    %eax,(%dx)
  104c05:	ba fc 0c 00 00       	mov    $0xcfc,%edx
  104c0a:	ed                   	in     (%dx),%eax
  104c0b:	83 e1 02             	and    $0x2,%ecx
  104c0e:	c1 e1 03             	shl    $0x3,%ecx
  104c11:	d3 e8                	shr    %cl,%eax
  104c13:	c3                   	ret
  104c14:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  104c1b:	00 
  104c1c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00104c20 <pci_read8>:
  104c20:	8b 4c 24 10          	mov    0x10(%esp),%ecx
  104c24:	0f b6 44 24 04       	movzbl 0x4(%esp),%eax
  104c29:	89 ca                	mov    %ecx,%edx
  104c2b:	c1 e0 10             	shl    $0x10,%eax
  104c2e:	81 e2 fc 00 00 00    	and    $0xfc,%edx
  104c34:	09 d0                	or     %edx,%eax
  104c36:	0f b6 54 24 08       	movzbl 0x8(%esp),%edx
  104c3b:	c1 e2 0b             	shl    $0xb,%edx
  104c3e:	09 d0                	or     %edx,%eax
  104c40:	0f b6 54 24 0c       	movzbl 0xc(%esp),%edx
  104c45:	c1 e2 08             	shl    $0x8,%edx
  104c48:	09 d0                	or     %edx,%eax
  104c4a:	ba f8 0c 00 00       	mov    $0xcf8,%edx
  104c4f:	0d 00 00 00 80       	or     $0x80000000,%eax
  104c54:	ef                   	out    %eax,(%dx)
  104c55:	ba fc 0c 00 00       	mov    $0xcfc,%edx
  104c5a:	ed                   	in     (%dx),%eax
  104c5b:	83 e1 03             	and    $0x3,%ecx
  104c5e:	c1 e1 03             	shl    $0x3,%ecx
  104c61:	d3 e8                	shr    %cl,%eax
  104c63:	c3                   	ret
  104c64:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  104c6b:	00 
  104c6c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00104c70 <pci_write32>:
  104c70:	0f b6 44 24 04       	movzbl 0x4(%esp),%eax
  104c75:	8b 54 24 10          	mov    0x10(%esp),%edx
  104c79:	c1 e0 10             	shl    $0x10,%eax
  104c7c:	81 e2 fc 00 00 00    	and    $0xfc,%edx
  104c82:	09 d0                	or     %edx,%eax
  104c84:	0f b6 54 24 0c       	movzbl 0xc(%esp),%edx
  104c89:	c1 e2 08             	shl    $0x8,%edx
  104c8c:	09 d0                	or     %edx,%eax
  104c8e:	0f b6 54 24 08       	movzbl 0x8(%esp),%edx
  104c93:	c1 e2 0b             	shl    $0xb,%edx
  104c96:	09 d0                	or     %edx,%eax
  104c98:	ba f8 0c 00 00       	mov    $0xcf8,%edx
  104c9d:	0d 00 00 00 80       	or     $0x80000000,%eax
  104ca2:	ef                   	out    %eax,(%dx)
  104ca3:	ba fc 0c 00 00       	mov    $0xcfc,%edx
  104ca8:	8b 44 24 14          	mov    0x14(%esp),%eax
  104cac:	ef                   	out    %eax,(%dx)
  104cad:	c3                   	ret
  104cae:	66 90                	xchg   %ax,%ax

00104cb0 <pci_init>:
  104cb0:	55                   	push   %ebp
  104cb1:	bd f8 0c 00 00       	mov    $0xcf8,%ebp
  104cb6:	57                   	push   %edi
  104cb7:	56                   	push   %esi
  104cb8:	53                   	push   %ebx
  104cb9:	83 ec 40             	sub    $0x40,%esp
  104cbc:	c7 05 80 7a 11 00 00 	movl   $0x0,0x117a80
  104cc3:	00 00 00 
  104cc6:	68 00 0a 00 00       	push   $0xa00
  104ccb:	6a 00                	push   $0x0
  104ccd:	68 a0 7a 11 00       	push   $0x117aa0
  104cd2:	e8 89 c8 ff ff       	call   101560 <memset>
  104cd7:	83 c4 10             	add    $0x10,%esp
  104cda:	8b 0d 80 7a 11 00    	mov    0x117a80,%ecx
  104ce0:	31 d2                	xor    %edx,%edx
  104ce2:	c6 44 24 14 00       	movb   $0x0,0x14(%esp)
  104ce7:	89 ce                	mov    %ecx,%esi
  104ce9:	89 d0                	mov    %edx,%eax
  104ceb:	31 ff                	xor    %edi,%edi
  104ced:	89 54 24 28          	mov    %edx,0x28(%esp)
  104cf1:	c1 e0 10             	shl    $0x10,%eax
  104cf4:	89 7c 24 1c          	mov    %edi,0x1c(%esp)
  104cf8:	89 44 24 24          	mov    %eax,0x24(%esp)
  104cfc:	89 4c 24 2c          	mov    %ecx,0x2c(%esp)
  104d00:	8b 5c 24 1c          	mov    0x1c(%esp),%ebx
  104d04:	8b 7c 24 24          	mov    0x24(%esp),%edi
  104d08:	89 ea                	mov    %ebp,%edx
  104d0a:	89 d8                	mov    %ebx,%eax
  104d0c:	c1 e0 0b             	shl    $0xb,%eax
  104d0f:	09 c7                	or     %eax,%edi
  104d11:	89 f8                	mov    %edi,%eax
  104d13:	89 7c 24 0c          	mov    %edi,0xc(%esp)
  104d17:	0d 00 00 00 80       	or     $0x80000000,%eax
  104d1c:	ef                   	out    %eax,(%dx)
  104d1d:	b9 fc 0c 00 00       	mov    $0xcfc,%ecx
  104d22:	89 ca                	mov    %ecx,%edx
  104d24:	ed                   	in     (%dx),%eax
  104d25:	66 83 f8 ff          	cmp    $0xffff,%ax
  104d29:	0f 84 7d 01 00 00    	je     104eac <pci_init+0x1fc>
  104d2f:	89 f8                	mov    %edi,%eax
  104d31:	89 ea                	mov    %ebp,%edx
  104d33:	0d 0c 00 00 80       	or     $0x8000000c,%eax
  104d38:	ef                   	out    %eax,(%dx)
  104d39:	89 ca                	mov    %ecx,%edx
  104d3b:	ed                   	in     (%dx),%eax
  104d3c:	c1 e8 10             	shr    $0x10,%eax
  104d3f:	0f b6 4c 24 28       	movzbl 0x28(%esp),%ecx
  104d44:	88 5c 24 22          	mov    %bl,0x22(%esp)
  104d48:	bf fc 0c 00 00       	mov    $0xcfc,%edi
  104d4d:	c0 f8 07             	sar    $0x7,%al
  104d50:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
  104d57:	00 
  104d58:	89 f3                	mov    %esi,%ebx
  104d5a:	83 e0 07             	and    $0x7,%eax
  104d5d:	88 4c 24 23          	mov    %cl,0x23(%esp)
  104d61:	83 c0 01             	add    $0x1,%eax
  104d64:	0f b6 c0             	movzbl %al,%eax
  104d67:	89 44 24 10          	mov    %eax,0x10(%esp)
  104d6b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  104d70:	8b 4c 24 04          	mov    0x4(%esp),%ecx
  104d74:	8b 44 24 0c          	mov    0xc(%esp),%eax
  104d78:	89 ea                	mov    %ebp,%edx
  104d7a:	c1 e1 08             	shl    $0x8,%ecx
  104d7d:	09 c1                	or     %eax,%ecx
  104d7f:	89 ce                	mov    %ecx,%esi
  104d81:	81 ce 00 00 00 80    	or     $0x80000000,%esi
  104d87:	89 f0                	mov    %esi,%eax
  104d89:	ef                   	out    %eax,(%dx)
  104d8a:	89 fa                	mov    %edi,%edx
  104d8c:	ed                   	in     (%dx),%eax
  104d8d:	89 44 24 08          	mov    %eax,0x8(%esp)
  104d91:	66 83 7c 24 08 ff    	cmpw   $0xffff,0x8(%esp)
  104d97:	0f 84 fa 00 00 00    	je     104e97 <pci_init+0x1e7>
  104d9d:	83 fb 3f             	cmp    $0x3f,%ebx
  104da0:	0f 87 f1 00 00 00    	ja     104e97 <pci_init+0x1e7>
  104da6:	8d 14 9b             	lea    (%ebx,%ebx,4),%edx
  104da9:	8d 43 01             	lea    0x1(%ebx),%eax
  104dac:	89 44 24 18          	mov    %eax,0x18(%esp)
  104db0:	8d 04 d5 00 00 00 00 	lea    0x0(,%edx,8),%eax
  104db7:	8d 98 a0 7a 11 00    	lea    0x117aa0(%eax),%ebx
  104dbd:	89 44 24 14          	mov    %eax,0x14(%esp)
  104dc1:	0f b6 44 24 23       	movzbl 0x23(%esp),%eax
  104dc6:	88 04 d5 a0 7a 11 00 	mov    %al,0x117aa0(,%edx,8)
  104dcd:	0f b6 44 24 22       	movzbl 0x22(%esp),%eax
  104dd2:	89 ea                	mov    %ebp,%edx
  104dd4:	88 43 01             	mov    %al,0x1(%ebx)
  104dd7:	0f b6 44 24 04       	movzbl 0x4(%esp),%eax
  104ddc:	88 43 02             	mov    %al,0x2(%ebx)
  104ddf:	0f b7 44 24 08       	movzwl 0x8(%esp),%eax
  104de4:	66 89 43 04          	mov    %ax,0x4(%ebx)
  104de8:	89 f0                	mov    %esi,%eax
  104dea:	ef                   	out    %eax,(%dx)
  104deb:	89 fa                	mov    %edi,%edx
  104ded:	ed                   	in     (%dx),%eax
  104dee:	89 ce                	mov    %ecx,%esi
  104df0:	c1 e8 10             	shr    $0x10,%eax
  104df3:	89 ea                	mov    %ebp,%edx
  104df5:	81 ce 08 00 00 80    	or     $0x80000008,%esi
  104dfb:	66 89 43 06          	mov    %ax,0x6(%ebx)
  104dff:	89 f0                	mov    %esi,%eax
  104e01:	ef                   	out    %eax,(%dx)
  104e02:	89 fa                	mov    %edi,%edx
  104e04:	ed                   	in     (%dx),%eax
  104e05:	c1 e8 18             	shr    $0x18,%eax
  104e08:	89 ea                	mov    %ebp,%edx
  104e0a:	88 43 08             	mov    %al,0x8(%ebx)
  104e0d:	89 f0                	mov    %esi,%eax
  104e0f:	ef                   	out    %eax,(%dx)
  104e10:	89 fa                	mov    %edi,%edx
  104e12:	ed                   	in     (%dx),%eax
  104e13:	c1 e8 10             	shr    $0x10,%eax
  104e16:	89 ea                	mov    %ebp,%edx
  104e18:	88 43 09             	mov    %al,0x9(%ebx)
  104e1b:	89 f0                	mov    %esi,%eax
  104e1d:	ef                   	out    %eax,(%dx)
  104e1e:	89 fa                	mov    %edi,%edx
  104e20:	ed                   	in     (%dx),%eax
  104e21:	88 63 0a             	mov    %ah,0xa(%ebx)
  104e24:	89 ea                	mov    %ebp,%edx
  104e26:	89 f0                	mov    %esi,%eax
  104e28:	ef                   	out    %eax,(%dx)
  104e29:	89 fa                	mov    %edi,%edx
  104e2b:	ed                   	in     (%dx),%eax
  104e2c:	89 ca                	mov    %ecx,%edx
  104e2e:	88 43 0b             	mov    %al,0xb(%ebx)
  104e31:	81 ca 0c 00 00 80    	or     $0x8000000c,%edx
  104e37:	89 d0                	mov    %edx,%eax
  104e39:	89 ea                	mov    %ebp,%edx
  104e3b:	ef                   	out    %eax,(%dx)
  104e3c:	89 fa                	mov    %edi,%edx
  104e3e:	ed                   	in     (%dx),%eax
  104e3f:	89 ce                	mov    %ecx,%esi
  104e41:	c1 e8 10             	shr    $0x10,%eax
  104e44:	89 ea                	mov    %ebp,%edx
  104e46:	81 ce 3c 00 00 80    	or     $0x8000003c,%esi
  104e4c:	88 43 0c             	mov    %al,0xc(%ebx)
  104e4f:	89 f0                	mov    %esi,%eax
  104e51:	ef                   	out    %eax,(%dx)
  104e52:	89 fa                	mov    %edi,%edx
  104e54:	ed                   	in     (%dx),%eax
  104e55:	88 43 0d             	mov    %al,0xd(%ebx)
  104e58:	89 ea                	mov    %ebp,%edx
  104e5a:	89 f0                	mov    %esi,%eax
  104e5c:	ef                   	out    %eax,(%dx)
  104e5d:	89 fa                	mov    %edi,%edx
  104e5f:	ed                   	in     (%dx),%eax
  104e60:	8b 74 24 14          	mov    0x14(%esp),%esi
  104e64:	88 63 0e             	mov    %ah,0xe(%ebx)
  104e67:	bb 10 00 00 00       	mov    $0x10,%ebx
  104e6c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  104e70:	89 c8                	mov    %ecx,%eax
  104e72:	89 ea                	mov    %ebp,%edx
  104e74:	09 d8                	or     %ebx,%eax
  104e76:	0d 00 00 00 80       	or     $0x80000000,%eax
  104e7b:	ef                   	out    %eax,(%dx)
  104e7c:	89 fa                	mov    %edi,%edx
  104e7e:	ed                   	in     (%dx),%eax
  104e7f:	89 84 1e a0 7a 11 00 	mov    %eax,0x117aa0(%esi,%ebx,1)
  104e86:	83 c3 04             	add    $0x4,%ebx
  104e89:	83 fb 28             	cmp    $0x28,%ebx
  104e8c:	75 e2                	jne    104e70 <pci_init+0x1c0>
  104e8e:	c6 44 24 14 01       	movb   $0x1,0x14(%esp)
  104e93:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  104e97:	83 44 24 04 01       	addl   $0x1,0x4(%esp)
  104e9c:	8b 44 24 04          	mov    0x4(%esp),%eax
  104ea0:	39 44 24 10          	cmp    %eax,0x10(%esp)
  104ea4:	0f 85 c6 fe ff ff    	jne    104d70 <pci_init+0xc0>
  104eaa:	89 de                	mov    %ebx,%esi
  104eac:	83 44 24 1c 01       	addl   $0x1,0x1c(%esp)
  104eb1:	8b 44 24 1c          	mov    0x1c(%esp),%eax
  104eb5:	83 f8 20             	cmp    $0x20,%eax
  104eb8:	0f 85 42 fe ff ff    	jne    104d00 <pci_init+0x50>
  104ebe:	8b 54 24 28          	mov    0x28(%esp),%edx
  104ec2:	8b 4c 24 2c          	mov    0x2c(%esp),%ecx
  104ec6:	83 c2 01             	add    $0x1,%edx
  104ec9:	81 fa 00 01 00 00    	cmp    $0x100,%edx
  104ecf:	0f 85 14 fe ff ff    	jne    104ce9 <pci_init+0x39>
  104ed5:	80 7c 24 14 00       	cmpb   $0x0,0x14(%esp)
  104eda:	74 08                	je     104ee4 <pci_init+0x234>
  104edc:	89 35 80 7a 11 00    	mov    %esi,0x117a80
  104ee2:	89 f1                	mov    %esi,%ecx
  104ee4:	83 ec 04             	sub    $0x4,%esp
  104ee7:	51                   	push   %ecx
  104ee8:	68 a4 93 10 00       	push   $0x1093a4
  104eed:	6a 01                	push   $0x1
  104eef:	e8 3c fc ff ff       	call   104b30 <klog>
  104ef4:	83 c4 4c             	add    $0x4c,%esp
  104ef7:	5b                   	pop    %ebx
  104ef8:	5e                   	pop    %esi
  104ef9:	5f                   	pop    %edi
  104efa:	5d                   	pop    %ebp
  104efb:	c3                   	ret
  104efc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00104f00 <pci_find_class>:
  104f00:	8b 0d 80 7a 11 00    	mov    0x117a80,%ecx
  104f06:	57                   	push   %edi
  104f07:	56                   	push   %esi
  104f08:	53                   	push   %ebx
  104f09:	0f b6 7c 24 10       	movzbl 0x10(%esp),%edi
  104f0e:	0f b6 74 24 14       	movzbl 0x14(%esp),%esi
  104f13:	85 c9                	test   %ecx,%ecx
  104f15:	74 31                	je     104f48 <pci_find_class+0x48>
  104f17:	b8 a8 7a 11 00       	mov    $0x117aa8,%eax
  104f1c:	31 d2                	xor    %edx,%edx
  104f1e:	eb 0a                	jmp    104f2a <pci_find_class+0x2a>
  104f20:	83 c2 01             	add    $0x1,%edx
  104f23:	83 c0 28             	add    $0x28,%eax
  104f26:	39 ca                	cmp    %ecx,%edx
  104f28:	74 1e                	je     104f48 <pci_find_class+0x48>
  104f2a:	89 fb                	mov    %edi,%ebx
  104f2c:	38 18                	cmp    %bl,(%eax)
  104f2e:	75 f0                	jne    104f20 <pci_find_class+0x20>
  104f30:	89 f3                	mov    %esi,%ebx
  104f32:	38 58 01             	cmp    %bl,0x1(%eax)
  104f35:	75 e9                	jne    104f20 <pci_find_class+0x20>
  104f37:	8d 04 92             	lea    (%edx,%edx,4),%eax
  104f3a:	5b                   	pop    %ebx
  104f3b:	5e                   	pop    %esi
  104f3c:	8d 04 c5 a0 7a 11 00 	lea    0x117aa0(,%eax,8),%eax
  104f43:	5f                   	pop    %edi
  104f44:	c3                   	ret
  104f45:	8d 76 00             	lea    0x0(%esi),%esi
  104f48:	5b                   	pop    %ebx
  104f49:	31 c0                	xor    %eax,%eax
  104f4b:	5e                   	pop    %esi
  104f4c:	5f                   	pop    %edi
  104f4d:	c3                   	ret
  104f4e:	66 90                	xchg   %ax,%ax

00104f50 <pci_find_device>:
  104f50:	8b 0d 80 7a 11 00    	mov    0x117a80,%ecx
  104f56:	56                   	push   %esi
  104f57:	53                   	push   %ebx
  104f58:	0f b7 5c 24 0c       	movzwl 0xc(%esp),%ebx
  104f5d:	0f b7 74 24 10       	movzwl 0x10(%esp),%esi
  104f62:	85 c9                	test   %ecx,%ecx
  104f64:	74 32                	je     104f98 <pci_find_device+0x48>
  104f66:	b8 a4 7a 11 00       	mov    $0x117aa4,%eax
  104f6b:	31 d2                	xor    %edx,%edx
  104f6d:	eb 0b                	jmp    104f7a <pci_find_device+0x2a>
  104f6f:	90                   	nop
  104f70:	83 c2 01             	add    $0x1,%edx
  104f73:	83 c0 28             	add    $0x28,%eax
  104f76:	39 ca                	cmp    %ecx,%edx
  104f78:	74 1e                	je     104f98 <pci_find_device+0x48>
  104f7a:	66 39 18             	cmp    %bx,(%eax)
  104f7d:	75 f1                	jne    104f70 <pci_find_device+0x20>
  104f7f:	66 39 70 02          	cmp    %si,0x2(%eax)
  104f83:	75 eb                	jne    104f70 <pci_find_device+0x20>
  104f85:	8d 04 92             	lea    (%edx,%edx,4),%eax
  104f88:	5b                   	pop    %ebx
  104f89:	5e                   	pop    %esi
  104f8a:	8d 04 c5 a0 7a 11 00 	lea    0x117aa0(,%eax,8),%eax
  104f91:	c3                   	ret
  104f92:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  104f98:	31 c0                	xor    %eax,%eax
  104f9a:	5b                   	pop    %ebx
  104f9b:	5e                   	pop    %esi
  104f9c:	c3                   	ret
  104f9d:	8d 76 00             	lea    0x0(%esi),%esi

00104fa0 <pci_get_devices>:
  104fa0:	b8 a0 7a 11 00       	mov    $0x117aa0,%eax
  104fa5:	c3                   	ret
  104fa6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  104fad:	00 
  104fae:	66 90                	xchg   %ax,%ax

00104fb0 <pci_device_count>:
  104fb0:	a1 80 7a 11 00       	mov    0x117a80,%eax
  104fb5:	c3                   	ret
  104fb6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  104fbd:	00 
  104fbe:	66 90                	xchg   %ax,%ax

00104fc0 <pci_dump>:
  104fc0:	56                   	push   %esi
  104fc1:	53                   	push   %ebx
  104fc2:	83 ec 08             	sub    $0x8,%esp
  104fc5:	ff 35 80 7a 11 00    	push   0x117a80
  104fcb:	68 b4 9b 10 00       	push   $0x109bb4
  104fd0:	6a 01                	push   $0x1
  104fd2:	e8 59 fb ff ff       	call   104b30 <klog>
  104fd7:	a1 80 7a 11 00       	mov    0x117a80,%eax
  104fdc:	83 c4 10             	add    $0x10,%esp
  104fdf:	85 c0                	test   %eax,%eax
  104fe1:	74 55                	je     105038 <pci_dump+0x78>
  104fe3:	bb a0 7a 11 00       	mov    $0x117aa0,%ebx
  104fe8:	31 f6                	xor    %esi,%esi
  104fea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  104ff0:	0f b6 43 0d          	movzbl 0xd(%ebx),%eax
  104ff4:	83 ec 08             	sub    $0x8,%esp
  104ff7:	83 c6 01             	add    $0x1,%esi
  104ffa:	83 c3 28             	add    $0x28,%ebx
  104ffd:	50                   	push   %eax
  104ffe:	0f b6 43 e1          	movzbl -0x1f(%ebx),%eax
  105002:	50                   	push   %eax
  105003:	0f b6 43 e0          	movzbl -0x20(%ebx),%eax
  105007:	50                   	push   %eax
  105008:	0f b7 43 de          	movzwl -0x22(%ebx),%eax
  10500c:	50                   	push   %eax
  10500d:	0f b7 43 dc          	movzwl -0x24(%ebx),%eax
  105011:	50                   	push   %eax
  105012:	0f b6 43 da          	movzbl -0x26(%ebx),%eax
  105016:	50                   	push   %eax
  105017:	0f b6 43 d9          	movzbl -0x27(%ebx),%eax
  10501b:	50                   	push   %eax
  10501c:	0f b6 43 d8          	movzbl -0x28(%ebx),%eax
  105020:	50                   	push   %eax
  105021:	68 d4 9b 10 00       	push   $0x109bd4
  105026:	6a 01                	push   $0x1
  105028:	e8 03 fb ff ff       	call   104b30 <klog>
  10502d:	83 c4 30             	add    $0x30,%esp
  105030:	3b 35 80 7a 11 00    	cmp    0x117a80,%esi
  105036:	72 b8                	jb     104ff0 <pci_dump+0x30>
  105038:	83 c4 04             	add    $0x4,%esp
  10503b:	5b                   	pop    %ebx
  10503c:	5e                   	pop    %esi
  10503d:	c3                   	ret
  10503e:	66 90                	xchg   %ax,%ax

00105040 <kbd_irq_handler>:
  105040:	e4 64                	in     $0x64,%al
  105042:	a8 01                	test   $0x1,%al
  105044:	74 2a                	je     105070 <kbd_irq_handler+0x30>
  105046:	55                   	push   %ebp
  105047:	57                   	push   %edi
  105048:	56                   	push   %esi
  105049:	53                   	push   %ebx
  10504a:	83 ec 04             	sub    $0x4,%esp
  10504d:	e4 60                	in     $0x60,%al
  10504f:	89 c3                	mov    %eax,%ebx
  105051:	0f b6 15 a4 84 11 00 	movzbl 0x1184a4,%edx
  105058:	83 e3 7f             	and    $0x7f,%ebx
  10505b:	8d 4b e3             	lea    -0x1d(%ebx),%ecx
  10505e:	80 f9 1d             	cmp    $0x1d,%cl
  105061:	77 15                	ja     105078 <kbd_irq_handler+0x38>
  105063:	0f b6 c9             	movzbl %cl,%ecx
  105066:	ff 24 8d c0 a9 10 00 	jmp    *0x10a9c0(,%ecx,4)
  10506d:	8d 76 00             	lea    0x0(%esi),%esi
  105070:	c3                   	ret
  105071:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  105078:	84 c0                	test   %al,%al
  10507a:	78 64                	js     1050e0 <kbd_irq_handler+0xa0>
  10507c:	83 3d a0 84 11 00 01 	cmpl   $0x1,0x1184a0
  105083:	b9 40 ab 10 00       	mov    $0x10ab40,%ecx
  105088:	b8 40 aa 10 00       	mov    $0x10aa40,%eax
  10508d:	0f 45 c1             	cmovne %ecx,%eax
  105090:	8b 0d a8 84 11 00    	mov    0x1184a8,%ecx
  105096:	89 c6                	mov    %eax,%esi
  105098:	8d 41 01             	lea    0x1(%ecx),%eax
  10509b:	83 e0 3f             	and    $0x3f,%eax
  10509e:	89 04 24             	mov    %eax,(%esp)
  1050a1:	3b 05 ac 84 11 00    	cmp    0x1184ac,%eax
  1050a7:	74 37                	je     1050e0 <kbd_irq_handler+0xa0>
  1050a9:	89 d7                	mov    %edx,%edi
  1050ab:	0f b6 eb             	movzbl %bl,%ebp
  1050ae:	88 1c 8d c0 84 11 00 	mov    %bl,0x1184c0(,%ecx,4)
  1050b5:	83 e7 09             	and    $0x9,%edi
  1050b8:	8d 34 6e             	lea    (%esi,%ebp,2),%esi
  1050bb:	89 f8                	mov    %edi,%eax
  1050bd:	3c 01                	cmp    $0x1,%al
  1050bf:	83 de ff             	sbb    $0xffffffff,%esi
  1050c2:	0f b6 3e             	movzbl (%esi),%edi
  1050c5:	8d 34 8d c0 84 11 00 	lea    0x1184c0(,%ecx,4),%esi
  1050cc:	88 56 02             	mov    %dl,0x2(%esi)
  1050cf:	89 f8                	mov    %edi,%eax
  1050d1:	c6 46 03 00          	movb   $0x0,0x3(%esi)
  1050d5:	88 46 01             	mov    %al,0x1(%esi)
  1050d8:	8b 04 24             	mov    (%esp),%eax
  1050db:	a3 a8 84 11 00       	mov    %eax,0x1184a8
  1050e0:	83 c4 04             	add    $0x4,%esp
  1050e3:	5b                   	pop    %ebx
  1050e4:	5e                   	pop    %esi
  1050e5:	5f                   	pop    %edi
  1050e6:	5d                   	pop    %ebp
  1050e7:	c3                   	ret
  1050e8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1050ef:	00 
  1050f0:	89 d1                	mov    %edx,%ecx
  1050f2:	83 e2 fe             	and    $0xfffffffe,%edx
  1050f5:	83 c9 01             	or     $0x1,%ecx
  1050f8:	84 c0                	test   %al,%al
  1050fa:	0f 49 d1             	cmovns %ecx,%edx
  1050fd:	88 15 a4 84 11 00    	mov    %dl,0x1184a4
  105103:	83 c4 04             	add    $0x4,%esp
  105106:	5b                   	pop    %ebx
  105107:	5e                   	pop    %esi
  105108:	5f                   	pop    %edi
  105109:	5d                   	pop    %ebp
  10510a:	c3                   	ret
  10510b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  105110:	89 d1                	mov    %edx,%ecx
  105112:	83 e2 fd             	and    $0xfffffffd,%edx
  105115:	83 c9 02             	or     $0x2,%ecx
  105118:	84 c0                	test   %al,%al
  10511a:	0f 49 d1             	cmovns %ecx,%edx
  10511d:	88 15 a4 84 11 00    	mov    %dl,0x1184a4
  105123:	83 c4 04             	add    $0x4,%esp
  105126:	5b                   	pop    %ebx
  105127:	5e                   	pop    %esi
  105128:	5f                   	pop    %edi
  105129:	5d                   	pop    %ebp
  10512a:	c3                   	ret
  10512b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  105130:	84 c0                	test   %al,%al
  105132:	78 ac                	js     1050e0 <kbd_irq_handler+0xa0>
  105134:	83 f2 08             	xor    $0x8,%edx
  105137:	88 15 a4 84 11 00    	mov    %dl,0x1184a4
  10513d:	eb a1                	jmp    1050e0 <kbd_irq_handler+0xa0>
  10513f:	90                   	nop
  105140:	89 d1                	mov    %edx,%ecx
  105142:	83 e2 fb             	and    $0xfffffffb,%edx
  105145:	83 c9 04             	or     $0x4,%ecx
  105148:	84 c0                	test   %al,%al
  10514a:	0f 49 d1             	cmovns %ecx,%edx
  10514d:	88 15 a4 84 11 00    	mov    %dl,0x1184a4
  105153:	83 c4 04             	add    $0x4,%esp
  105156:	5b                   	pop    %ebx
  105157:	5e                   	pop    %esi
  105158:	5f                   	pop    %edi
  105159:	5d                   	pop    %ebp
  10515a:	c3                   	ret
  10515b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00105160 <kbd_init>:
  105160:	83 ec 14             	sub    $0x14,%esp
  105163:	c6 05 a4 84 11 00 00 	movb   $0x0,0x1184a4
  10516a:	68 40 50 10 00       	push   $0x105040
  10516f:	6a 01                	push   $0x1
  105171:	c7 05 a8 84 11 00 00 	movl   $0x0,0x1184a8
  105178:	00 00 00 
  10517b:	c7 05 ac 84 11 00 00 	movl   $0x0,0x1184ac
  105182:	00 00 00 
  105185:	c7 05 a0 84 11 00 00 	movl   $0x0,0x1184a0
  10518c:	00 00 00 
  10518f:	e8 fc d5 ff ff       	call   102790 <irq_register>
  105194:	58                   	pop    %eax
  105195:	5a                   	pop    %edx
  105196:	68 0c 9c 10 00       	push   $0x109c0c
  10519b:	6a 01                	push   $0x1
  10519d:	e8 8e f9 ff ff       	call   104b30 <klog>
  1051a2:	83 c4 1c             	add    $0x1c,%esp
  1051a5:	c3                   	ret
  1051a6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1051ad:	00 
  1051ae:	66 90                	xchg   %ax,%ax

001051b0 <kbd_set_layout>:
  1051b0:	83 ec 10             	sub    $0x10,%esp
  1051b3:	ba bd 93 10 00       	mov    $0x1093bd,%edx
  1051b8:	8b 44 24 14          	mov    0x14(%esp),%eax
  1051bc:	83 f8 01             	cmp    $0x1,%eax
  1051bf:	a3 a0 84 11 00       	mov    %eax,0x1184a0
  1051c4:	b8 c0 93 10 00       	mov    $0x1093c0,%eax
  1051c9:	0f 44 c2             	cmove  %edx,%eax
  1051cc:	50                   	push   %eax
  1051cd:	68 c3 93 10 00       	push   $0x1093c3
  1051d2:	6a 01                	push   $0x1
  1051d4:	e8 57 f9 ff ff       	call   104b30 <klog>
  1051d9:	83 c4 1c             	add    $0x1c,%esp
  1051dc:	c3                   	ret
  1051dd:	8d 76 00             	lea    0x0(%esi),%esi

001051e0 <kbd_poll>:
  1051e0:	a1 ac 84 11 00       	mov    0x1184ac,%eax
  1051e5:	31 d2                	xor    %edx,%edx
  1051e7:	3b 05 a8 84 11 00    	cmp    0x1184a8,%eax
  1051ed:	74 1d                	je     10520c <kbd_poll+0x2c>
  1051ef:	8b 0c 85 c0 84 11 00 	mov    0x1184c0(,%eax,4),%ecx
  1051f6:	83 c0 01             	add    $0x1,%eax
  1051f9:	8b 54 24 04          	mov    0x4(%esp),%edx
  1051fd:	83 e0 3f             	and    $0x3f,%eax
  105200:	a3 ac 84 11 00       	mov    %eax,0x1184ac
  105205:	89 0a                	mov    %ecx,(%edx)
  105207:	ba 01 00 00 00       	mov    $0x1,%edx
  10520c:	89 d0                	mov    %edx,%eax
  10520e:	c3                   	ret
  10520f:	90                   	nop

00105210 <kbd_has_input>:
  105210:	a1 a8 84 11 00       	mov    0x1184a8,%eax
  105215:	39 05 ac 84 11 00    	cmp    %eax,0x1184ac
  10521b:	0f 95 c0             	setne  %al
  10521e:	c3                   	ret
  10521f:	90                   	nop

00105220 <kbd_getchar>:
  105220:	83 ec 0c             	sub    $0xc,%esp
  105223:	a1 ac 84 11 00       	mov    0x1184ac,%eax
  105228:	39 05 a8 84 11 00    	cmp    %eax,0x1184a8
  10522e:	74 20                	je     105250 <kbd_getchar+0x30>
  105230:	0f b6 14 85 c1 84 11 	movzbl 0x1184c1(,%eax,4),%edx
  105237:	00 
  105238:	83 c0 01             	add    $0x1,%eax
  10523b:	83 e0 3f             	and    $0x3f,%eax
  10523e:	a3 ac 84 11 00       	mov    %eax,0x1184ac
  105243:	89 d0                	mov    %edx,%eax
  105245:	83 c4 0c             	add    $0xc,%esp
  105248:	c3                   	ret
  105249:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  105250:	e8 4b 10 00 00       	call   1062a0 <task_yield>
  105255:	a1 ac 84 11 00       	mov    0x1184ac,%eax
  10525a:	3b 05 a8 84 11 00    	cmp    0x1184a8,%eax
  105260:	75 ce                	jne    105230 <kbd_getchar+0x10>
  105262:	eb ec                	jmp    105250 <kbd_getchar+0x30>
  105264:	66 90                	xchg   %ax,%ax
  105266:	66 90                	xchg   %ax,%ax
  105268:	66 90                	xchg   %ax,%ax
  10526a:	66 90                	xchg   %ax,%ax
  10526c:	66 90                	xchg   %ax,%ax
  10526e:	66 90                	xchg   %ax,%ax

00105270 <rtc_init>:
  105270:	83 ec 14             	sub    $0x14,%esp
  105273:	68 34 9c 10 00       	push   $0x109c34
  105278:	6a 01                	push   $0x1
  10527a:	e8 b1 f8 ff ff       	call   104b30 <klog>
  10527f:	83 c4 1c             	add    $0x1c,%esp
  105282:	c3                   	ret
  105283:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10528a:	00 
  10528b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00105290 <rtc_read>:
  105290:	55                   	push   %ebp
  105291:	ba 0a 00 00 00       	mov    $0xa,%edx
  105296:	57                   	push   %edi
  105297:	56                   	push   %esi
  105298:	53                   	push   %ebx
  105299:	83 ec 0c             	sub    $0xc,%esp
  10529c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  1052a0:	89 d0                	mov    %edx,%eax
  1052a2:	e6 70                	out    %al,$0x70
  1052a4:	e4 71                	in     $0x71,%al
  1052a6:	84 c0                	test   %al,%al
  1052a8:	78 f6                	js     1052a0 <rtc_read+0x10>
  1052aa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1052b0:	31 c0                	xor    %eax,%eax
  1052b2:	e6 70                	out    %al,$0x70
  1052b4:	e4 71                	in     $0x71,%al
  1052b6:	88 44 24 03          	mov    %al,0x3(%esp)
  1052ba:	89 c1                	mov    %eax,%ecx
  1052bc:	b8 02 00 00 00       	mov    $0x2,%eax
  1052c1:	e6 70                	out    %al,$0x70
  1052c3:	e4 71                	in     $0x71,%al
  1052c5:	88 44 24 04          	mov    %al,0x4(%esp)
  1052c9:	89 c7                	mov    %eax,%edi
  1052cb:	b8 04 00 00 00       	mov    $0x4,%eax
  1052d0:	e6 70                	out    %al,$0x70
  1052d2:	e4 71                	in     $0x71,%al
  1052d4:	88 44 24 02          	mov    %al,0x2(%esp)
  1052d8:	89 c6                	mov    %eax,%esi
  1052da:	b8 07 00 00 00       	mov    $0x7,%eax
  1052df:	e6 70                	out    %al,$0x70
  1052e1:	e4 71                	in     $0x71,%al
  1052e3:	88 44 24 05          	mov    %al,0x5(%esp)
  1052e7:	89 c3                	mov    %eax,%ebx
  1052e9:	b8 08 00 00 00       	mov    $0x8,%eax
  1052ee:	e6 70                	out    %al,$0x70
  1052f0:	e4 71                	in     $0x71,%al
  1052f2:	88 44 24 09          	mov    %al,0x9(%esp)
  1052f6:	88 44 24 06          	mov    %al,0x6(%esp)
  1052fa:	b8 09 00 00 00       	mov    $0x9,%eax
  1052ff:	e6 70                	out    %al,$0x70
  105301:	e4 71                	in     $0x71,%al
  105303:	88 44 24 0a          	mov    %al,0xa(%esp)
  105307:	88 44 24 07          	mov    %al,0x7(%esp)
  10530b:	b8 06 00 00 00       	mov    $0x6,%eax
  105310:	e6 70                	out    %al,$0x70
  105312:	e4 71                	in     $0x71,%al
  105314:	88 44 24 08          	mov    %al,0x8(%esp)
  105318:	89 c5                	mov    %eax,%ebp
  10531a:	ba 0a 00 00 00       	mov    $0xa,%edx
  10531f:	90                   	nop
  105320:	89 d0                	mov    %edx,%eax
  105322:	e6 70                	out    %al,$0x70
  105324:	e4 71                	in     $0x71,%al
  105326:	84 c0                	test   %al,%al
  105328:	78 f6                	js     105320 <rtc_read+0x90>
  10532a:	31 c0                	xor    %eax,%eax
  10532c:	e6 70                	out    %al,$0x70
  10532e:	e4 71                	in     $0x71,%al
  105330:	38 c1                	cmp    %al,%cl
  105332:	0f 85 78 ff ff ff    	jne    1052b0 <rtc_read+0x20>
  105338:	b8 0b 00 00 00       	mov    $0xb,%eax
  10533d:	e6 70                	out    %al,$0x70
  10533f:	e4 71                	in     $0x71,%al
  105341:	88 44 24 0b          	mov    %al,0xb(%esp)
  105345:	a8 04                	test   $0x4,%al
  105347:	0f 85 8e 00 00 00    	jne    1053db <rtc_read+0x14b>
  10534d:	89 c8                	mov    %ecx,%eax
  10534f:	83 e1 0f             	and    $0xf,%ecx
  105352:	c0 e8 04             	shr    $0x4,%al
  105355:	8d 04 80             	lea    (%eax,%eax,4),%eax
  105358:	8d 04 41             	lea    (%ecx,%eax,2),%eax
  10535b:	88 44 24 03          	mov    %al,0x3(%esp)
  10535f:	89 f8                	mov    %edi,%eax
  105361:	83 e7 0f             	and    $0xf,%edi
  105364:	c0 e8 04             	shr    $0x4,%al
  105367:	8d 04 80             	lea    (%eax,%eax,4),%eax
  10536a:	8d 04 47             	lea    (%edi,%eax,2),%eax
  10536d:	88 44 24 04          	mov    %al,0x4(%esp)
  105371:	89 f0                	mov    %esi,%eax
  105373:	83 e6 0f             	and    $0xf,%esi
  105376:	c0 e8 04             	shr    $0x4,%al
  105379:	8d 04 80             	lea    (%eax,%eax,4),%eax
  10537c:	8d 04 46             	lea    (%esi,%eax,2),%eax
  10537f:	88 44 24 02          	mov    %al,0x2(%esp)
  105383:	89 d8                	mov    %ebx,%eax
  105385:	83 e3 0f             	and    $0xf,%ebx
  105388:	c0 e8 04             	shr    $0x4,%al
  10538b:	8d 04 80             	lea    (%eax,%eax,4),%eax
  10538e:	8d 04 43             	lea    (%ebx,%eax,2),%eax
  105391:	0f b6 5c 24 09       	movzbl 0x9(%esp),%ebx
  105396:	88 44 24 05          	mov    %al,0x5(%esp)
  10539a:	89 d8                	mov    %ebx,%eax
  10539c:	83 e3 0f             	and    $0xf,%ebx
  10539f:	c0 e8 04             	shr    $0x4,%al
  1053a2:	89 da                	mov    %ebx,%edx
  1053a4:	0f b6 5c 24 0a       	movzbl 0xa(%esp),%ebx
  1053a9:	8d 04 80             	lea    (%eax,%eax,4),%eax
  1053ac:	8d 04 42             	lea    (%edx,%eax,2),%eax
  1053af:	88 44 24 06          	mov    %al,0x6(%esp)
  1053b3:	89 d8                	mov    %ebx,%eax
  1053b5:	83 e3 0f             	and    $0xf,%ebx
  1053b8:	c0 e8 04             	shr    $0x4,%al
  1053bb:	89 da                	mov    %ebx,%edx
  1053bd:	8d 04 80             	lea    (%eax,%eax,4),%eax
  1053c0:	8d 04 42             	lea    (%edx,%eax,2),%eax
  1053c3:	89 ea                	mov    %ebp,%edx
  1053c5:	88 44 24 07          	mov    %al,0x7(%esp)
  1053c9:	89 e8                	mov    %ebp,%eax
  1053cb:	83 e2 0f             	and    $0xf,%edx
  1053ce:	c0 e8 04             	shr    $0x4,%al
  1053d1:	8d 04 80             	lea    (%eax,%eax,4),%eax
  1053d4:	8d 04 42             	lea    (%edx,%eax,2),%eax
  1053d7:	88 44 24 08          	mov    %al,0x8(%esp)
  1053db:	f6 44 24 0b 02       	testb  $0x2,0xb(%esp)
  1053e0:	75 09                	jne    1053eb <rtc_read+0x15b>
  1053e2:	0f b6 44 24 02       	movzbl 0x2(%esp),%eax
  1053e7:	84 c0                	test   %al,%al
  1053e9:	78 48                	js     105433 <rtc_read+0x1a3>
  1053eb:	8b 74 24 20          	mov    0x20(%esp),%esi
  1053ef:	0f b6 44 24 03       	movzbl 0x3(%esp),%eax
  1053f4:	88 46 06             	mov    %al,0x6(%esi)
  1053f7:	0f b6 44 24 04       	movzbl 0x4(%esp),%eax
  1053fc:	88 46 05             	mov    %al,0x5(%esi)
  1053ff:	0f b6 44 24 02       	movzbl 0x2(%esp),%eax
  105404:	88 46 04             	mov    %al,0x4(%esi)
  105407:	0f b6 44 24 05       	movzbl 0x5(%esp),%eax
  10540c:	88 46 03             	mov    %al,0x3(%esi)
  10540f:	0f b6 44 24 06       	movzbl 0x6(%esp),%eax
  105414:	88 46 02             	mov    %al,0x2(%esi)
  105417:	0f b6 44 24 07       	movzbl 0x7(%esp),%eax
  10541c:	66 05 d0 07          	add    $0x7d0,%ax
  105420:	66 89 06             	mov    %ax,(%esi)
  105423:	0f b6 44 24 08       	movzbl 0x8(%esp),%eax
  105428:	88 46 07             	mov    %al,0x7(%esi)
  10542b:	83 c4 0c             	add    $0xc,%esp
  10542e:	5b                   	pop    %ebx
  10542f:	5e                   	pop    %esi
  105430:	5f                   	pop    %edi
  105431:	5d                   	pop    %ebp
  105432:	c3                   	ret
  105433:	83 e0 7f             	and    $0x7f,%eax
  105436:	83 c0 0c             	add    $0xc,%eax
  105439:	0f b6 c8             	movzbl %al,%ecx
  10543c:	b8 ab aa aa aa       	mov    $0xaaaaaaab,%eax
  105441:	f7 e1                	mul    %ecx
  105443:	89 cb                	mov    %ecx,%ebx
  105445:	c1 ea 04             	shr    $0x4,%edx
  105448:	8d 04 52             	lea    (%edx,%edx,2),%eax
  10544b:	c1 e0 03             	shl    $0x3,%eax
  10544e:	29 c3                	sub    %eax,%ebx
  105450:	88 5c 24 02          	mov    %bl,0x2(%esp)
  105454:	eb 95                	jmp    1053eb <rtc_read+0x15b>
  105456:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10545d:	00 
  10545e:	66 90                	xchg   %ax,%ax

00105460 <rtc_dump>:
  105460:	83 ec 1c             	sub    $0x1c,%esp
  105463:	8d 44 24 08          	lea    0x8(%esp),%eax
  105467:	50                   	push   %eax
  105468:	e8 23 fe ff ff       	call   105290 <rtc_read>
  10546d:	0f b6 44 24 12       	movzbl 0x12(%esp),%eax
  105472:	89 04 24             	mov    %eax,(%esp)
  105475:	0f b6 44 24 11       	movzbl 0x11(%esp),%eax
  10547a:	50                   	push   %eax
  10547b:	0f b6 44 24 14       	movzbl 0x14(%esp),%eax
  105480:	50                   	push   %eax
  105481:	0f b6 44 24 17       	movzbl 0x17(%esp),%eax
  105486:	50                   	push   %eax
  105487:	0f b6 44 24 1a       	movzbl 0x1a(%esp),%eax
  10548c:	50                   	push   %eax
  10548d:	0f b7 44 24 1c       	movzwl 0x1c(%esp),%eax
  105492:	50                   	push   %eax
  105493:	68 58 9c 10 00       	push   $0x109c58
  105498:	6a 01                	push   $0x1
  10549a:	e8 91 f6 ff ff       	call   104b30 <klog>
  10549f:	83 c4 3c             	add    $0x3c,%esp
  1054a2:	c3                   	ret
  1054a3:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1054aa:	00 
  1054ab:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

001054b0 <rtc_to_unix>:
  1054b0:	57                   	push   %edi
  1054b1:	56                   	push   %esi
  1054b2:	53                   	push   %ebx
  1054b3:	8b 4c 24 10          	mov    0x10(%esp),%ecx
  1054b7:	0f b7 01             	movzwl (%ecx),%eax
  1054ba:	0f b6 79 02          	movzbl 0x2(%ecx),%edi
  1054be:	8d 90 4e f8 ff ff    	lea    -0x7b2(%eax),%edx
  1054c4:	89 fb                	mov    %edi,%ebx
  1054c6:	0f b7 bc 3f 3e ac 10 	movzwl 0x10ac3e(%edi,%edi,1),%edi
  1054cd:	00 
  1054ce:	89 c6                	mov    %eax,%esi
  1054d0:	69 d2 6d 01 00 00    	imul   $0x16d,%edx,%edx
  1054d6:	2d b0 07 00 00       	sub    $0x7b0,%eax
  1054db:	c1 e8 02             	shr    $0x2,%eax
  1054de:	01 fa                	add    %edi,%edx
  1054e0:	01 c2                	add    %eax,%edx
  1054e2:	80 fb 02             	cmp    $0x2,%bl
  1054e5:	76 1a                	jbe    105501 <rtc_to_unix+0x51>
  1054e7:	f7 c6 03 00 00 00    	test   $0x3,%esi
  1054ed:	75 41                	jne    105530 <rtc_to_unix+0x80>
  1054ef:	66 69 c6 29 5c       	imul   $0x5c29,%si,%ax
  1054f4:	66 c1 c8 02          	ror    $0x2,%ax
  1054f8:	66 3d 8f 02          	cmp    $0x28f,%ax
  1054fc:	76 32                	jbe    105530 <rtc_to_unix+0x80>
  1054fe:	83 c2 01             	add    $0x1,%edx
  105501:	0f b6 41 04          	movzbl 0x4(%ecx),%eax
  105505:	0f b6 59 05          	movzbl 0x5(%ecx),%ebx
  105509:	69 c0 10 0e 00 00    	imul   $0xe10,%eax,%eax
  10550f:	6b db 3c             	imul   $0x3c,%ebx,%ebx
  105512:	01 d8                	add    %ebx,%eax
  105514:	0f b6 59 06          	movzbl 0x6(%ecx),%ebx
  105518:	0f b6 49 03          	movzbl 0x3(%ecx),%ecx
  10551c:	8d 54 0a ff          	lea    -0x1(%edx,%ecx,1),%edx
  105520:	01 d8                	add    %ebx,%eax
  105522:	5b                   	pop    %ebx
  105523:	5e                   	pop    %esi
  105524:	69 d2 80 51 01 00    	imul   $0x15180,%edx,%edx
  10552a:	5f                   	pop    %edi
  10552b:	01 d0                	add    %edx,%eax
  10552d:	c3                   	ret
  10552e:	66 90                	xchg   %ax,%ax
  105530:	66 69 f6 29 5c       	imul   $0x5c29,%si,%si
  105535:	66 c1 ce 04          	ror    $0x4,%si
  105539:	66 81 fe a3 00       	cmp    $0xa3,%si
  10553e:	77 c1                	ja     105501 <rtc_to_unix+0x51>
  105540:	83 c2 01             	add    $0x1,%edx
  105543:	eb bc                	jmp    105501 <rtc_to_unix+0x51>
  105545:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10554c:	00 
  10554d:	8d 76 00             	lea    0x0(%esi),%esi

00105550 <hpet_init>:
  105550:	57                   	push   %edi
  105551:	56                   	push   %esi
  105552:	53                   	push   %ebx
  105553:	8b 5c 24 10          	mov    0x10(%esp),%ebx
  105557:	83 ec 04             	sub    $0x4,%esp
  10555a:	6a 13                	push   $0x13
  10555c:	53                   	push   %ebx
  10555d:	53                   	push   %ebx
  10555e:	e8 dd db ff ff       	call   103140 <paging_map>
  105563:	8b 03                	mov    (%ebx),%eax
  105565:	8b 53 04             	mov    0x4(%ebx),%edx
  105568:	b9 ff e0 f5 05       	mov    $0x5f5e0ff,%ecx
  10556d:	89 1d c8 85 11 00    	mov    %ebx,0x1185c8
  105573:	89 d0                	mov    %edx,%eax
  105575:	31 d2                	xor    %edx,%edx
  105577:	89 c6                	mov    %eax,%esi
  105579:	89 d7                	mov    %edx,%edi
  10557b:	a3 c0 85 11 00       	mov    %eax,0x1185c0
  105580:	83 c6 ff             	add    $0xffffffff,%esi
  105583:	89 15 c4 85 11 00    	mov    %edx,0x1185c4
  105589:	83 d7 ff             	adc    $0xffffffff,%edi
  10558c:	83 c4 10             	add    $0x10,%esp
  10558f:	39 f1                	cmp    %esi,%ecx
  105591:	b9 00 00 00 00       	mov    $0x0,%ecx
  105596:	19 f9                	sbb    %edi,%ecx
  105598:	72 46                	jb     1055e0 <hpet_init+0x90>
  10559a:	8b 73 10             	mov    0x10(%ebx),%esi
  10559d:	8b 7b 14             	mov    0x14(%ebx),%edi
  1055a0:	83 ec 0c             	sub    $0xc,%esp
  1055a3:	83 ce 01             	or     $0x1,%esi
  1055a6:	89 73 10             	mov    %esi,0x10(%ebx)
  1055a9:	89 7b 14             	mov    %edi,0x14(%ebx)
  1055ac:	52                   	push   %edx
  1055ad:	50                   	push   %eax
  1055ae:	83 ec 0c             	sub    $0xc,%esp
  1055b1:	52                   	push   %edx
  1055b2:	50                   	push   %eax
  1055b3:	68 7e 8d 03 00       	push   $0x38d7e
  1055b8:	68 00 80 c6 a4       	push   $0xa4c68000
  1055bd:	e8 fe c8 ff ff       	call   101ec0 <__udivdi3>
  1055c2:	83 c4 1c             	add    $0x1c,%esp
  1055c5:	50                   	push   %eax
  1055c6:	68 b0 9c 10 00       	push   $0x109cb0
  1055cb:	6a 01                	push   $0x1
  1055cd:	e8 5e f5 ff ff       	call   104b30 <klog>
  1055d2:	83 c4 20             	add    $0x20,%esp
  1055d5:	5b                   	pop    %ebx
  1055d6:	5e                   	pop    %esi
  1055d7:	5f                   	pop    %edi
  1055d8:	c3                   	ret
  1055d9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  1055e0:	52                   	push   %edx
  1055e1:	50                   	push   %eax
  1055e2:	68 7c 9c 10 00       	push   $0x109c7c
  1055e7:	6a 02                	push   $0x2
  1055e9:	e8 42 f5 ff ff       	call   104b30 <klog>
  1055ee:	83 c4 10             	add    $0x10,%esp
  1055f1:	c7 05 c8 85 11 00 00 	movl   $0x0,0x1185c8
  1055f8:	00 00 00 
  1055fb:	5b                   	pop    %ebx
  1055fc:	5e                   	pop    %esi
  1055fd:	5f                   	pop    %edi
  1055fe:	c3                   	ret
  1055ff:	90                   	nop

00105600 <hpet_read_counter>:
  105600:	a1 c8 85 11 00       	mov    0x1185c8,%eax
  105605:	85 c0                	test   %eax,%eax
  105607:	74 17                	je     105620 <hpet_read_counter+0x20>
  105609:	8b 90 f4 00 00 00    	mov    0xf4(%eax),%edx
  10560f:	8b 80 f0 00 00 00    	mov    0xf0(%eax),%eax
  105615:	c3                   	ret
  105616:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10561d:	00 
  10561e:	66 90                	xchg   %ax,%ax
  105620:	31 c0                	xor    %eax,%eax
  105622:	31 d2                	xor    %edx,%edx
  105624:	c3                   	ret
  105625:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10562c:	00 
  10562d:	8d 76 00             	lea    0x0(%esi),%esi

00105630 <hpet_freq_hz>:
  105630:	56                   	push   %esi
  105631:	53                   	push   %ebx
  105632:	83 ec 04             	sub    $0x4,%esp
  105635:	8b 0d c0 85 11 00    	mov    0x1185c0,%ecx
  10563b:	8b 1d c4 85 11 00    	mov    0x1185c4,%ebx
  105641:	89 ca                	mov    %ecx,%edx
  105643:	09 da                	or     %ebx,%edx
  105645:	74 21                	je     105668 <hpet_freq_hz+0x38>
  105647:	53                   	push   %ebx
  105648:	51                   	push   %ecx
  105649:	68 7e 8d 03 00       	push   $0x38d7e
  10564e:	68 00 80 c6 a4       	push   $0xa4c68000
  105653:	e8 68 c8 ff ff       	call   101ec0 <__udivdi3>
  105658:	83 c4 10             	add    $0x10,%esp
  10565b:	83 c4 04             	add    $0x4,%esp
  10565e:	5b                   	pop    %ebx
  10565f:	5e                   	pop    %esi
  105660:	c3                   	ret
  105661:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  105668:	83 c4 04             	add    $0x4,%esp
  10566b:	31 c0                	xor    %eax,%eax
  10566d:	5b                   	pop    %ebx
  10566e:	5e                   	pop    %esi
  10566f:	c3                   	ret

00105670 <hpet_sleep_us>:
  105670:	55                   	push   %ebp
  105671:	57                   	push   %edi
  105672:	56                   	push   %esi
  105673:	53                   	push   %ebx
  105674:	83 ec 2c             	sub    $0x2c,%esp
  105677:	8b 1d c8 85 11 00    	mov    0x1185c8,%ebx
  10567d:	8b 44 24 40          	mov    0x40(%esp),%eax
  105681:	8b 54 24 44          	mov    0x44(%esp),%edx
  105685:	89 04 24             	mov    %eax,(%esp)
  105688:	89 54 24 04          	mov    %edx,0x4(%esp)
  10568c:	85 db                	test   %ebx,%ebx
  10568e:	0f 84 8c 00 00 00    	je     105720 <hpet_sleep_us+0xb0>
  105694:	8b 35 c0 85 11 00    	mov    0x1185c0,%esi
  10569a:	8b 3d c4 85 11 00    	mov    0x1185c4,%edi
  1056a0:	89 f1                	mov    %esi,%ecx
  1056a2:	09 f9                	or     %edi,%ecx
  1056a4:	74 7a                	je     105720 <hpet_sleep_us+0xb0>
  1056a6:	8b 83 f0 00 00 00    	mov    0xf0(%ebx),%eax
  1056ac:	8b 93 f4 00 00 00    	mov    0xf4(%ebx),%edx
  1056b2:	bd 00 ca 9a 3b       	mov    $0x3b9aca00,%ebp
  1056b7:	89 44 24 08          	mov    %eax,0x8(%esp)
  1056bb:	8b 04 24             	mov    (%esp),%eax
  1056be:	89 54 24 0c          	mov    %edx,0xc(%esp)
  1056c2:	8b 54 24 04          	mov    0x4(%esp),%edx
  1056c6:	57                   	push   %edi
  1056c7:	69 ca 00 ca 9a 3b    	imul   $0x3b9aca00,%edx,%ecx
  1056cd:	56                   	push   %esi
  1056ce:	f7 e5                	mul    %ebp
  1056d0:	01 ca                	add    %ecx,%edx
  1056d2:	52                   	push   %edx
  1056d3:	50                   	push   %eax
  1056d4:	e8 e7 c7 ff ff       	call   101ec0 <__udivdi3>
  1056d9:	83 c4 10             	add    $0x10,%esp
  1056dc:	03 44 24 08          	add    0x8(%esp),%eax
  1056e0:	13 54 24 0c          	adc    0xc(%esp),%edx
  1056e4:	eb 16                	jmp    1056fc <hpet_sleep_us+0x8c>
  1056e6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1056ed:	00 
  1056ee:	66 90                	xchg   %ax,%ax
  1056f0:	f3 90                	pause
  1056f2:	8b 1d c8 85 11 00    	mov    0x1185c8,%ebx
  1056f8:	85 db                	test   %ebx,%ebx
  1056fa:	74 f4                	je     1056f0 <hpet_sleep_us+0x80>
  1056fc:	8b 8b f0 00 00 00    	mov    0xf0(%ebx),%ecx
  105702:	8b 9b f4 00 00 00    	mov    0xf4(%ebx),%ebx
  105708:	39 c1                	cmp    %eax,%ecx
  10570a:	89 df                	mov    %ebx,%edi
  10570c:	19 d7                	sbb    %edx,%edi
  10570e:	72 e0                	jb     1056f0 <hpet_sleep_us+0x80>
  105710:	83 c4 2c             	add    $0x2c,%esp
  105713:	5b                   	pop    %ebx
  105714:	5e                   	pop    %esi
  105715:	5f                   	pop    %edi
  105716:	5d                   	pop    %ebp
  105717:	c3                   	ret
  105718:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10571f:	00 
  105720:	8b 34 24             	mov    (%esp),%esi
  105723:	8b 7c 24 04          	mov    0x4(%esp),%edi
  105727:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
  10572e:	00 
  10572f:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
  105736:	00 
  105737:	89 f0                	mov    %esi,%eax
  105739:	89 fa                	mov    %edi,%edx
  10573b:	01 f0                	add    %esi,%eax
  10573d:	0f a4 f2 01          	shld   $0x1,%esi,%edx
  105741:	89 c1                	mov    %eax,%ecx
  105743:	89 d3                	mov    %edx,%ebx
  105745:	89 f0                	mov    %esi,%eax
  105747:	89 fa                	mov    %edi,%edx
  105749:	01 f1                	add    %esi,%ecx
  10574b:	11 fb                	adc    %edi,%ebx
  10574d:	0f a4 cb 03          	shld   $0x3,%ecx,%ebx
  105751:	c1 e1 03             	shl    $0x3,%ecx
  105754:	01 c8                	add    %ecx,%eax
  105756:	11 da                	adc    %ebx,%edx
  105758:	0f a4 c2 02          	shld   $0x2,%eax,%edx
  10575c:	c1 e0 02             	shl    $0x2,%eax
  10575f:	89 c3                	mov    %eax,%ebx
  105761:	89 d1                	mov    %edx,%ecx
  105763:	eb 1b                	jmp    105780 <hpet_sleep_us+0x110>
  105765:	8d 76 00             	lea    0x0(%esi),%esi
  105768:	f3 90                	pause
  10576a:	8b 44 24 18          	mov    0x18(%esp),%eax
  10576e:	8b 54 24 1c          	mov    0x1c(%esp),%edx
  105772:	83 c0 01             	add    $0x1,%eax
  105775:	83 d2 00             	adc    $0x0,%edx
  105778:	89 44 24 18          	mov    %eax,0x18(%esp)
  10577c:	89 54 24 1c          	mov    %edx,0x1c(%esp)
  105780:	8b 44 24 18          	mov    0x18(%esp),%eax
  105784:	8b 54 24 1c          	mov    0x1c(%esp),%edx
  105788:	39 d8                	cmp    %ebx,%eax
  10578a:	89 d0                	mov    %edx,%eax
  10578c:	19 c8                	sbb    %ecx,%eax
  10578e:	72 d8                	jb     105768 <hpet_sleep_us+0xf8>
  105790:	83 c4 2c             	add    $0x2c,%esp
  105793:	5b                   	pop    %ebx
  105794:	5e                   	pop    %esi
  105795:	5f                   	pop    %edi
  105796:	5d                   	pop    %ebp
  105797:	c3                   	ret
  105798:	66 90                	xchg   %ax,%ax
  10579a:	66 90                	xchg   %ax,%ax
  10579c:	66 90                	xchg   %ax,%ax
  10579e:	66 90                	xchg   %ax,%ax

001057a0 <rsdt_find_table>:
  1057a0:	55                   	push   %ebp
  1057a1:	57                   	push   %edi
  1057a2:	56                   	push   %esi
  1057a3:	53                   	push   %ebx
  1057a4:	83 ec 0c             	sub    $0xc,%esp
  1057a7:	8b 1d d0 85 11 00    	mov    0x1185d0,%ebx
  1057ad:	85 db                	test   %ebx,%ebx
  1057af:	74 39                	je     1057ea <rsdt_find_table+0x4a>
  1057b1:	89 c7                	mov    %eax,%edi
  1057b3:	8b 43 04             	mov    0x4(%ebx),%eax
  1057b6:	83 c3 24             	add    $0x24,%ebx
  1057b9:	83 e8 24             	sub    $0x24,%eax
  1057bc:	c1 e8 02             	shr    $0x2,%eax
  1057bf:	74 29                	je     1057ea <rsdt_find_table+0x4a>
  1057c1:	8d 2c 83             	lea    (%ebx,%eax,4),%ebp
  1057c4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  1057c8:	8b 03                	mov    (%ebx),%eax
  1057ca:	85 c0                	test   %eax,%eax
  1057cc:	74 15                	je     1057e3 <rsdt_find_table+0x43>
  1057ce:	83 ec 04             	sub    $0x4,%esp
  1057d1:	89 c6                	mov    %eax,%esi
  1057d3:	6a 04                	push   $0x4
  1057d5:	57                   	push   %edi
  1057d6:	50                   	push   %eax
  1057d7:	e8 64 be ff ff       	call   101640 <memcmp>
  1057dc:	83 c4 10             	add    $0x10,%esp
  1057df:	85 c0                	test   %eax,%eax
  1057e1:	74 09                	je     1057ec <rsdt_find_table+0x4c>
  1057e3:	83 c3 04             	add    $0x4,%ebx
  1057e6:	39 eb                	cmp    %ebp,%ebx
  1057e8:	75 de                	jne    1057c8 <rsdt_find_table+0x28>
  1057ea:	31 f6                	xor    %esi,%esi
  1057ec:	83 c4 0c             	add    $0xc,%esp
  1057ef:	89 f0                	mov    %esi,%eax
  1057f1:	5b                   	pop    %ebx
  1057f2:	5e                   	pop    %esi
  1057f3:	5f                   	pop    %edi
  1057f4:	5d                   	pop    %ebp
  1057f5:	c3                   	ret
  1057f6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1057fd:	00 
  1057fe:	66 90                	xchg   %ax,%ax

00105800 <acpi_init>:
  105800:	55                   	push   %ebp
  105801:	57                   	push   %edi
  105802:	56                   	push   %esi
  105803:	53                   	push   %ebx
  105804:	83 ec 20             	sub    $0x20,%esp
  105807:	6a 1c                	push   $0x1c
  105809:	6a 00                	push   $0x0
  10580b:	68 cc 85 11 00       	push   $0x1185cc
  105810:	e8 4b bd ff ff       	call   101560 <memset>
  105815:	8d 74 24 10          	lea    0x10(%esp),%esi
  105819:	c7 44 24 10 00 00 0e 	movl   $0xe0000,0x10(%esp)
  105820:	00 
  105821:	c7 44 24 14 ff ff 0f 	movl   $0xfffff,0x14(%esp)
  105828:	00 
  105829:	c7 44 24 18 00 00 08 	movl   $0x80000,0x18(%esp)
  105830:	00 
  105831:	c7 44 24 1c ff ff 09 	movl   $0x9ffff,0x1c(%esp)
  105838:	00 
  105839:	89 f4                	mov    %esi,%esp
  10583b:	8b 2e                	mov    (%esi),%ebp
  10583d:	8b 5e 04             	mov    0x4(%esi),%ebx
  105840:	39 dd                	cmp    %ebx,%ebp
  105842:	73 24                	jae    105868 <acpi_init+0x68>
  105844:	8d 7d 14             	lea    0x14(%ebp),%edi
  105847:	83 ec 04             	sub    $0x4,%esp
  10584a:	6a 08                	push   $0x8
  10584c:	68 dc 93 10 00       	push   $0x1093dc
  105851:	55                   	push   %ebp
  105852:	e8 e9 bd ff ff       	call   101640 <memcmp>
  105857:	83 c4 10             	add    $0x10,%esp
  10585a:	85 c0                	test   %eax,%eax
  10585c:	74 32                	je     105890 <acpi_init+0x90>
  10585e:	83 c5 10             	add    $0x10,%ebp
  105861:	83 c7 10             	add    $0x10,%edi
  105864:	39 dd                	cmp    %ebx,%ebp
  105866:	72 df                	jb     105847 <acpi_init+0x47>
  105868:	83 c6 08             	add    $0x8,%esi
  10586b:	8d 44 24 10          	lea    0x10(%esp),%eax
  10586f:	39 c6                	cmp    %eax,%esi
  105871:	75 c8                	jne    10583b <acpi_init+0x3b>
  105873:	83 ec 08             	sub    $0x8,%esp
  105876:	68 d0 9c 10 00       	push   $0x109cd0
  10587b:	6a 02                	push   $0x2
  10587d:	e8 ae f2 ff ff       	call   104b30 <klog>
  105882:	83 c4 10             	add    $0x10,%esp
  105885:	83 c4 1c             	add    $0x1c,%esp
  105888:	5b                   	pop    %ebx
  105889:	5e                   	pop    %esi
  10588a:	5f                   	pop    %edi
  10588b:	5d                   	pop    %ebp
  10588c:	c3                   	ret
  10588d:	8d 76 00             	lea    0x0(%esi),%esi
  105890:	89 e8                	mov    %ebp,%eax
  105892:	31 c9                	xor    %ecx,%ecx
  105894:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  105898:	02 08                	add    (%eax),%cl
  10589a:	83 c0 01             	add    $0x1,%eax
  10589d:	39 c7                	cmp    %eax,%edi
  10589f:	75 f7                	jne    105898 <acpi_init+0x98>
  1058a1:	84 c9                	test   %cl,%cl
  1058a3:	75 b9                	jne    10585e <acpi_init+0x5e>
  1058a5:	85 ed                	test   %ebp,%ebp
  1058a7:	74 ca                	je     105873 <acpi_init+0x73>
  1058a9:	8b 45 10             	mov    0x10(%ebp),%eax
  1058ac:	0f b6 55 0f          	movzbl 0xf(%ebp),%edx
  1058b0:	83 ec 0c             	sub    $0xc,%esp
  1058b3:	a3 d0 85 11 00       	mov    %eax,0x1185d0
  1058b8:	52                   	push   %edx
  1058b9:	50                   	push   %eax
  1058ba:	55                   	push   %ebp
  1058bb:	68 00 9d 10 00       	push   $0x109d00
  1058c0:	6a 01                	push   $0x1
  1058c2:	e8 69 f2 ff ff       	call   104b30 <klog>
  1058c7:	83 c4 20             	add    $0x20,%esp
  1058ca:	b8 e5 93 10 00       	mov    $0x1093e5,%eax
  1058cf:	e8 cc fe ff ff       	call   1057a0 <rsdt_find_table>
  1058d4:	85 c0                	test   %eax,%eax
  1058d6:	0f 84 8c 00 00 00    	je     105968 <acpi_init+0x168>
  1058dc:	8b 50 40             	mov    0x40(%eax),%edx
  1058df:	8b 48 44             	mov    0x44(%eax),%ecx
  1058e2:	83 ec 0c             	sub    $0xc,%esp
  1058e5:	a3 d4 85 11 00       	mov    %eax,0x1185d4
  1058ea:	c7 05 e4 85 11 00 00 	movl   $0x14001400,0x1185e4
  1058f1:	14 00 14 
  1058f4:	66 89 15 e0 85 11 00 	mov    %dx,0x1185e0
  1058fb:	0f b7 d2             	movzwl %dx,%edx
  1058fe:	66 89 0d e2 85 11 00 	mov    %cx,0x1185e2
  105905:	0f b7 c9             	movzwl %cx,%ecx
  105908:	51                   	push   %ecx
  105909:	52                   	push   %edx
  10590a:	50                   	push   %eax
  10590b:	68 28 9d 10 00       	push   $0x109d28
  105910:	6a 01                	push   $0x1
  105912:	e8 19 f2 ff ff       	call   104b30 <klog>
  105917:	83 c4 20             	add    $0x20,%esp
  10591a:	b8 05 94 10 00       	mov    $0x109405,%eax
  10591f:	e8 7c fe ff ff       	call   1057a0 <rsdt_find_table>
  105924:	85 c0                	test   %eax,%eax
  105926:	74 1b                	je     105943 <acpi_init+0x143>
  105928:	8b 40 2c             	mov    0x2c(%eax),%eax
  10592b:	83 ec 04             	sub    $0x4,%esp
  10592e:	a3 d8 85 11 00       	mov    %eax,0x1185d8
  105933:	50                   	push   %eax
  105934:	68 0a 94 10 00       	push   $0x10940a
  105939:	6a 01                	push   $0x1
  10593b:	e8 f0 f1 ff ff       	call   104b30 <klog>
  105940:	83 c4 10             	add    $0x10,%esp
  105943:	83 ec 08             	sub    $0x8,%esp
  105946:	c6 05 cc 85 11 00 01 	movb   $0x1,0x1185cc
  10594d:	68 1f 94 10 00       	push   $0x10941f
  105952:	6a 01                	push   $0x1
  105954:	e8 d7 f1 ff ff       	call   104b30 <klog>
  105959:	83 c4 10             	add    $0x10,%esp
  10595c:	83 c4 1c             	add    $0x1c,%esp
  10595f:	5b                   	pop    %ebx
  105960:	5e                   	pop    %esi
  105961:	5f                   	pop    %edi
  105962:	5d                   	pop    %ebp
  105963:	c3                   	ret
  105964:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  105968:	83 ec 08             	sub    $0x8,%esp
  10596b:	68 ea 93 10 00       	push   $0x1093ea
  105970:	6a 02                	push   $0x2
  105972:	e8 b9 f1 ff ff       	call   104b30 <klog>
  105977:	83 c4 10             	add    $0x10,%esp
  10597a:	eb 9e                	jmp    10591a <acpi_init+0x11a>
  10597c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00105980 <acpi_get_state>:
  105980:	b8 cc 85 11 00       	mov    $0x1185cc,%eax
  105985:	c3                   	ret
  105986:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10598d:	00 
  10598e:	66 90                	xchg   %ax,%ax

00105990 <acpi_dump>:
  105990:	83 ec 14             	sub    $0x14,%esp
  105993:	68 34 94 10 00       	push   $0x109434
  105998:	6a 01                	push   $0x1
  10599a:	e8 91 f1 ff ff       	call   104b30 <klog>
  10599f:	58                   	pop    %eax
  1059a0:	ff 35 d8 85 11 00    	push   0x1185d8
  1059a6:	ff 35 d4 85 11 00    	push   0x1185d4
  1059ac:	ff 35 d0 85 11 00    	push   0x1185d0
  1059b2:	68 58 9d 10 00       	push   $0x109d58
  1059b7:	6a 01                	push   $0x1
  1059b9:	e8 72 f1 ff ff       	call   104b30 <klog>
  1059be:	0f b7 05 e2 85 11 00 	movzwl 0x1185e2,%eax
  1059c5:	83 c4 20             	add    $0x20,%esp
  1059c8:	50                   	push   %eax
  1059c9:	0f b7 05 e0 85 11 00 	movzwl 0x1185e0,%eax
  1059d0:	50                   	push   %eax
  1059d1:	68 43 94 10 00       	push   $0x109443
  1059d6:	6a 01                	push   $0x1
  1059d8:	e8 53 f1 ff ff       	call   104b30 <klog>
  1059dd:	83 c4 1c             	add    $0x1c,%esp
  1059e0:	c3                   	ret
  1059e1:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1059e8:	00 
  1059e9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

001059f0 <acpi_shutdown>:
  1059f0:	56                   	push   %esi
  1059f1:	5e                   	pop    %esi
  1059f2:	83 ec 14             	sub    $0x14,%esp
  1059f5:	6a 01                	push   $0x1
  1059f7:	6a 0f                	push   $0xf
  1059f9:	e8 c2 e9 ff ff       	call   1043c0 <vga_set_color>
  1059fe:	c7 04 24 84 9d 10 00 	movl   $0x109d84,(%esp)
  105a05:	e8 06 ec ff ff       	call   104610 <vga_puts>
  105a0a:	c7 04 24 5f 94 10 00 	movl   $0x10945f,(%esp)
  105a11:	e8 da ef ff ff       	call   1049f0 <serial_puts>
  105a16:	0f b7 15 e0 85 11 00 	movzwl 0x1185e0,%edx
  105a1d:	83 c4 10             	add    $0x10,%esp
  105a20:	66 85 d2             	test   %dx,%dx
  105a23:	74 18                	je     105a3d <acpi_shutdown+0x4d>
  105a25:	0f b7 05 e4 85 11 00 	movzwl 0x1185e4,%eax
  105a2c:	80 cc 20             	or     $0x20,%ah
  105a2f:	66 ef                	out    %ax,(%dx)
  105a31:	0f b7 15 e2 85 11 00 	movzwl 0x1185e2,%edx
  105a38:	66 85 d2             	test   %dx,%dx
  105a3b:	75 16                	jne    105a53 <acpi_shutdown+0x63>
  105a3d:	b8 00 20 00 00       	mov    $0x2000,%eax
  105a42:	ba 04 06 00 00       	mov    $0x604,%edx
  105a47:	66 ef                	out    %ax,(%dx)
  105a49:	fa                   	cli
  105a4a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  105a50:	f4                   	hlt
  105a51:	eb fd                	jmp    105a50 <acpi_shutdown+0x60>
  105a53:	0f b7 05 e6 85 11 00 	movzwl 0x1185e6,%eax
  105a5a:	80 cc 20             	or     $0x20,%ah
  105a5d:	66 ef                	out    %ax,(%dx)
  105a5f:	eb dc                	jmp    105a3d <acpi_shutdown+0x4d>
  105a61:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  105a68:	00 
  105a69:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00105a70 <acpi_reboot>:
  105a70:	56                   	push   %esi
  105a71:	5e                   	pop    %esi
  105a72:	83 ec 28             	sub    $0x28,%esp
  105a75:	68 70 94 10 00       	push   $0x109470
  105a7a:	e8 71 ef ff ff       	call   1049f0 <serial_puts>
  105a7f:	83 c4 10             	add    $0x10,%esp
  105a82:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  105a88:	e4 64                	in     $0x64,%al
  105a8a:	83 e0 02             	and    $0x2,%eax
  105a8d:	89 c2                	mov    %eax,%edx
  105a8f:	75 f7                	jne    105a88 <acpi_reboot+0x18>
  105a91:	b8 fe ff ff ff       	mov    $0xfffffffe,%eax
  105a96:	e6 64                	out    %al,$0x64
  105a98:	89 d0                	mov    %edx,%eax
  105a9a:	e6 80                	out    %al,$0x80
  105a9c:	31 c0                	xor    %eax,%eax
  105a9e:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
  105aa5:	00 
  105aa6:	66 89 44 24 0a       	mov    %ax,0xa(%esp)
  105aab:	8d 44 24 0a          	lea    0xa(%esp),%eax
  105aaf:	0f 01 18             	lidtl  (%eax)
  105ab2:	cc                   	int3
  105ab3:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  105ab8:	f4                   	hlt
  105ab9:	eb fd                	jmp    105ab8 <acpi_reboot+0x48>
  105abb:	66 90                	xchg   %ax,%ax
  105abd:	66 90                	xchg   %ax,%ax
  105abf:	90                   	nop

00105ac0 <ide_identify>:
  105ac0:	57                   	push   %edi
  105ac1:	56                   	push   %esi
  105ac2:	89 c6                	mov    %eax,%esi
  105ac4:	53                   	push   %ebx
  105ac5:	81 ec 00 02 00 00    	sub    $0x200,%esp
  105acb:	0f b7 58 02          	movzwl 0x2(%eax),%ebx
  105acf:	0f b6 40 06          	movzbl 0x6(%eax),%eax
  105ad3:	c1 e0 04             	shl    $0x4,%eax
  105ad6:	8d 53 06             	lea    0x6(%ebx),%edx
  105ad9:	83 c8 a0             	or     $0xffffffa0,%eax
  105adc:	ee                   	out    %al,(%dx)
  105add:	31 c9                	xor    %ecx,%ecx
  105adf:	89 c8                	mov    %ecx,%eax
  105ae1:	e6 80                	out    %al,$0x80
  105ae3:	8d 53 02             	lea    0x2(%ebx),%edx
  105ae6:	ee                   	out    %al,(%dx)
  105ae7:	8d 53 03             	lea    0x3(%ebx),%edx
  105aea:	ee                   	out    %al,(%dx)
  105aeb:	8d 53 04             	lea    0x4(%ebx),%edx
  105aee:	ee                   	out    %al,(%dx)
  105aef:	8d 53 05             	lea    0x5(%ebx),%edx
  105af2:	ee                   	out    %al,(%dx)
  105af3:	b8 ec ff ff ff       	mov    $0xffffffec,%eax
  105af8:	8d 53 07             	lea    0x7(%ebx),%edx
  105afb:	ee                   	out    %al,(%dx)
  105afc:	89 c8                	mov    %ecx,%eax
  105afe:	e6 80                	out    %al,$0x80
  105b00:	ec                   	in     (%dx),%al
  105b01:	89 c7                	mov    %eax,%edi
  105b03:	84 c0                	test   %al,%al
  105b05:	75 11                	jne    105b18 <ide_identify+0x58>
  105b07:	81 c4 00 02 00 00    	add    $0x200,%esp
  105b0d:	89 f8                	mov    %edi,%eax
  105b0f:	5b                   	pop    %ebx
  105b10:	5e                   	pop    %esi
  105b11:	5f                   	pop    %edi
  105b12:	c3                   	ret
  105b13:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  105b18:	ec                   	in     (%dx),%al
  105b19:	89 c7                	mov    %eax,%edi
  105b1b:	83 e7 01             	and    $0x1,%edi
  105b1e:	0f 85 97 00 00 00    	jne    105bbb <ide_identify+0xfb>
  105b24:	b9 ff ff 0f 00       	mov    $0xfffff,%ecx
  105b29:	eb 0f                	jmp    105b3a <ide_identify+0x7a>
  105b2b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  105b30:	83 e9 01             	sub    $0x1,%ecx
  105b33:	74 d2                	je     105b07 <ide_identify+0x47>
  105b35:	ec                   	in     (%dx),%al
  105b36:	a8 01                	test   $0x1,%al
  105b38:	75 cd                	jne    105b07 <ide_identify+0x47>
  105b3a:	84 c0                	test   %al,%al
  105b3c:	78 f2                	js     105b30 <ide_identify+0x70>
  105b3e:	a8 08                	test   $0x8,%al
  105b40:	74 ee                	je     105b30 <ide_identify+0x70>
  105b42:	89 e1                	mov    %esp,%ecx
  105b44:	8d bc 24 00 02 00 00 	lea    0x200(%esp),%edi
  105b4b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  105b50:	89 da                	mov    %ebx,%edx
  105b52:	66 ed                	in     (%dx),%ax
  105b54:	66 89 01             	mov    %ax,(%ecx)
  105b57:	83 c1 02             	add    $0x2,%ecx
  105b5a:	39 f9                	cmp    %edi,%ecx
  105b5c:	75 f2                	jne    105b50 <ide_identify+0x90>
  105b5e:	8b 44 24 78          	mov    0x78(%esp),%eax
  105b62:	89 46 08             	mov    %eax,0x8(%esi)
  105b65:	31 c0                	xor    %eax,%eax
  105b67:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  105b6e:	00 
  105b6f:	90                   	nop
  105b70:	0f b7 54 44 36       	movzwl 0x36(%esp,%eax,2),%edx
  105b75:	88 74 46 0c          	mov    %dh,0xc(%esi,%eax,2)
  105b79:	88 54 46 0d          	mov    %dl,0xd(%esi,%eax,2)
  105b7d:	83 c0 01             	add    $0x1,%eax
  105b80:	83 f8 14             	cmp    $0x14,%eax
  105b83:	75 eb                	jne    105b70 <ide_identify+0xb0>
  105b85:	c6 46 34 00          	movb   $0x0,0x34(%esi)
  105b89:	8d 46 33             	lea    0x33(%esi),%eax
  105b8c:	8d 56 0b             	lea    0xb(%esi),%edx
  105b8f:	eb 11                	jmp    105ba2 <ide_identify+0xe2>
  105b91:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  105b98:	c6 00 00             	movb   $0x0,(%eax)
  105b9b:	83 e8 01             	sub    $0x1,%eax
  105b9e:	39 c2                	cmp    %eax,%edx
  105ba0:	74 05                	je     105ba7 <ide_identify+0xe7>
  105ba2:	80 38 20             	cmpb   $0x20,(%eax)
  105ba5:	74 f1                	je     105b98 <ide_identify+0xd8>
  105ba7:	c6 06 01             	movb   $0x1,(%esi)
  105baa:	bf 01 00 00 00       	mov    $0x1,%edi
  105baf:	81 c4 00 02 00 00    	add    $0x200,%esp
  105bb5:	89 f8                	mov    %edi,%eax
  105bb7:	5b                   	pop    %ebx
  105bb8:	5e                   	pop    %esi
  105bb9:	5f                   	pop    %edi
  105bba:	c3                   	ret
  105bbb:	31 ff                	xor    %edi,%edi
  105bbd:	e9 45 ff ff ff       	jmp    105b07 <ide_identify+0x47>
  105bc2:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  105bc9:	00 
  105bca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00105bd0 <ide_init>:
  105bd0:	55                   	push   %ebp
  105bd1:	57                   	push   %edi
  105bd2:	56                   	push   %esi
  105bd3:	53                   	push   %ebx
  105bd4:	bb 04 00 00 00       	mov    $0x4,%ebx
  105bd9:	83 ec 30             	sub    $0x30,%esp
  105bdc:	68 e0 00 00 00       	push   $0xe0
  105be1:	6a 00                	push   $0x0
  105be3:	68 20 86 11 00       	push   $0x118620
  105be8:	e8 73 b9 ff ff       	call   101560 <memset>
  105bed:	ba f6 03 00 00       	mov    $0x3f6,%edx
  105bf2:	89 d8                	mov    %ebx,%eax
  105bf4:	c7 05 00 86 11 00 00 	movl   $0x0,0x118600
  105bfb:	00 00 00 
  105bfe:	ee                   	out    %al,(%dx)
  105bff:	31 c9                	xor    %ecx,%ecx
  105c01:	89 c8                	mov    %ecx,%eax
  105c03:	e6 80                	out    %al,$0x80
  105c05:	ee                   	out    %al,(%dx)
  105c06:	e6 80                	out    %al,$0x80
  105c08:	ba 76 03 00 00       	mov    $0x376,%edx
  105c0d:	89 d8                	mov    %ebx,%eax
  105c0f:	ee                   	out    %al,(%dx)
  105c10:	89 c8                	mov    %ecx,%eax
  105c12:	e6 80                	out    %al,$0x80
  105c14:	ee                   	out    %al,(%dx)
  105c15:	e6 80                	out    %al,$0x80
  105c17:	31 ed                	xor    %ebp,%ebp
  105c19:	c7 44 24 18 f0 01 f6 	movl   $0x3f601f0,0x18(%esp)
  105c20:	03 
  105c21:	8d 74 24 18          	lea    0x18(%esp),%esi
  105c25:	31 ff                	xor    %edi,%edi
  105c27:	c7 44 24 1c 00 00 f0 	movl   $0x1f00000,0x1c(%esp)
  105c2e:	01 
  105c2f:	bb 20 86 11 00       	mov    $0x118620,%ebx
  105c34:	c7 44 24 20 f6 03 01 	movl   $0x103f6,0x20(%esp)
  105c3b:	00 
  105c3c:	c7 44 24 24 70 01 76 	movl   $0x3760170,0x24(%esp)
  105c43:	03 
  105c44:	c7 44 24 28 00 01 70 	movl   $0x1700100,0x28(%esp)
  105c4b:	01 
  105c4c:	c7 44 24 2c 76 03 01 	movl   $0x1010376,0x2c(%esp)
  105c53:	01 
  105c54:	83 c4 10             	add    $0x10,%esp
  105c57:	0f b7 06             	movzwl (%esi),%eax
  105c5a:	66 89 43 02          	mov    %ax,0x2(%ebx)
  105c5e:	0f b7 46 02          	movzwl 0x2(%esi),%eax
  105c62:	66 89 43 04          	mov    %ax,0x4(%ebx)
  105c66:	0f b7 46 04          	movzwl 0x4(%esi),%eax
  105c6a:	66 89 43 06          	mov    %ax,0x6(%ebx)
  105c6e:	89 d8                	mov    %ebx,%eax
  105c70:	e8 4b fe ff ff       	call   105ac0 <ide_identify>
  105c75:	84 c0                	test   %al,%al
  105c77:	75 27                	jne    105ca0 <ide_init+0xd0>
  105c79:	83 c7 01             	add    $0x1,%edi
  105c7c:	83 c6 06             	add    $0x6,%esi
  105c7f:	83 c3 38             	add    $0x38,%ebx
  105c82:	83 ff 04             	cmp    $0x4,%edi
  105c85:	75 d0                	jne    105c57 <ide_init+0x87>
  105c87:	83 ec 04             	sub    $0x4,%esp
  105c8a:	55                   	push   %ebp
  105c8b:	68 7f 94 10 00       	push   $0x10947f
  105c90:	6a 01                	push   $0x1
  105c92:	e8 99 ee ff ff       	call   104b30 <klog>
  105c97:	83 c4 3c             	add    $0x3c,%esp
  105c9a:	5b                   	pop    %ebx
  105c9b:	5e                   	pop    %esi
  105c9c:	5f                   	pop    %edi
  105c9d:	5d                   	pop    %ebp
  105c9e:	c3                   	ret
  105c9f:	90                   	nop
  105ca0:	83 ec 0c             	sub    $0xc,%esp
  105ca3:	8d 43 0c             	lea    0xc(%ebx),%eax
  105ca6:	83 c5 01             	add    $0x1,%ebp
  105ca9:	50                   	push   %eax
  105caa:	8b 43 08             	mov    0x8(%ebx),%eax
  105cad:	89 2d 00 86 11 00    	mov    %ebp,0x118600
  105cb3:	c1 e8 0b             	shr    $0xb,%eax
  105cb6:	50                   	push   %eax
  105cb7:	57                   	push   %edi
  105cb8:	68 a8 9d 10 00       	push   $0x109da8
  105cbd:	6a 01                	push   $0x1
  105cbf:	e8 6c ee ff ff       	call   104b30 <klog>
  105cc4:	8b 2d 00 86 11 00    	mov    0x118600,%ebp
  105cca:	83 c4 20             	add    $0x20,%esp
  105ccd:	eb aa                	jmp    105c79 <ide_init+0xa9>
  105ccf:	90                   	nop

00105cd0 <ide_read>:
  105cd0:	55                   	push   %ebp
  105cd1:	57                   	push   %edi
  105cd2:	56                   	push   %esi
  105cd3:	53                   	push   %ebx
  105cd4:	83 ec 04             	sub    $0x4,%esp
  105cd7:	8b 74 24 20          	mov    0x20(%esp),%esi
  105cdb:	8b 4c 24 18          	mov    0x18(%esp),%ecx
  105cdf:	89 f0                	mov    %esi,%eax
  105ce1:	88 44 24 03          	mov    %al,0x3(%esp)
  105ce5:	80 f9 03             	cmp    $0x3,%cl
  105ce8:	0f 87 d4 00 00 00    	ja     105dc2 <ide_read+0xf2>
  105cee:	0f b6 c9             	movzbl %cl,%ecx
  105cf1:	6b c1 38             	imul   $0x38,%ecx,%eax
  105cf4:	80 b8 20 86 11 00 00 	cmpb   $0x0,0x118620(%eax)
  105cfb:	8d 90 20 86 11 00    	lea    0x118620(%eax),%edx
  105d01:	0f 84 bb 00 00 00    	je     105dc2 <ide_read+0xf2>
  105d07:	0f b7 98 22 86 11 00 	movzwl 0x118622(%eax),%ebx
  105d0e:	89 d0                	mov    %edx,%eax
  105d10:	bf 00 00 10 00       	mov    $0x100000,%edi
  105d15:	8d 6b 07             	lea    0x7(%ebx),%ebp
  105d18:	89 ea                	mov    %ebp,%edx
  105d1a:	89 c5                	mov    %eax,%ebp
  105d1c:	83 ef 01             	sub    $0x1,%edi
  105d1f:	74 09                	je     105d2a <ide_read+0x5a>
  105d21:	ec                   	in     (%dx),%al
  105d22:	a8 01                	test   $0x1,%al
  105d24:	75 04                	jne    105d2a <ide_read+0x5a>
  105d26:	84 c0                	test   %al,%al
  105d28:	78 f2                	js     105d1c <ide_read+0x4c>
  105d2a:	89 e8                	mov    %ebp,%eax
  105d2c:	89 d5                	mov    %edx,%ebp
  105d2e:	8b 54 24 1c          	mov    0x1c(%esp),%edx
  105d32:	0f b7 78 02          	movzwl 0x2(%eax),%edi
  105d36:	0f b6 40 06          	movzbl 0x6(%eax),%eax
  105d3a:	c1 ea 18             	shr    $0x18,%edx
  105d3d:	83 e2 0f             	and    $0xf,%edx
  105d40:	c1 e0 04             	shl    $0x4,%eax
  105d43:	09 d0                	or     %edx,%eax
  105d45:	8d 57 06             	lea    0x6(%edi),%edx
  105d48:	83 c8 e0             	or     $0xffffffe0,%eax
  105d4b:	ee                   	out    %al,(%dx)
  105d4c:	0f b6 44 24 03       	movzbl 0x3(%esp),%eax
  105d51:	8d 57 02             	lea    0x2(%edi),%edx
  105d54:	ee                   	out    %al,(%dx)
  105d55:	0f b6 44 24 1c       	movzbl 0x1c(%esp),%eax
  105d5a:	8d 57 03             	lea    0x3(%edi),%edx
  105d5d:	ee                   	out    %al,(%dx)
  105d5e:	8b 44 24 1c          	mov    0x1c(%esp),%eax
  105d62:	8d 57 04             	lea    0x4(%edi),%edx
  105d65:	c1 e8 08             	shr    $0x8,%eax
  105d68:	ee                   	out    %al,(%dx)
  105d69:	8b 44 24 1c          	mov    0x1c(%esp),%eax
  105d6d:	8d 57 05             	lea    0x5(%edi),%edx
  105d70:	c1 e8 10             	shr    $0x10,%eax
  105d73:	ee                   	out    %al,(%dx)
  105d74:	b8 20 00 00 00       	mov    $0x20,%eax
  105d79:	89 ea                	mov    %ebp,%edx
  105d7b:	ee                   	out    %al,(%dx)
  105d7c:	89 f0                	mov    %esi,%eax
  105d7e:	84 c0                	test   %al,%al
  105d80:	74 7e                	je     105e00 <ide_read+0x130>
  105d82:	8b 44 24 24          	mov    0x24(%esp),%eax
  105d86:	6b c9 38             	imul   $0x38,%ecx,%ecx
  105d89:	8d b8 00 02 00 00    	lea    0x200(%eax),%edi
  105d8f:	89 f0                	mov    %esi,%eax
  105d91:	0f b6 f0             	movzbl %al,%esi
  105d94:	8d a9 20 86 11 00    	lea    0x118620(%ecx),%ebp
  105d9a:	c1 e6 09             	shl    $0x9,%esi
  105d9d:	01 fe                	add    %edi,%esi
  105d9f:	90                   	nop
  105da0:	8d 53 07             	lea    0x7(%ebx),%edx
  105da3:	ec                   	in     (%dx),%al
  105da4:	a8 01                	test   $0x1,%al
  105da6:	75 1a                	jne    105dc2 <ide_read+0xf2>
  105da8:	b9 ff ff 0f 00       	mov    $0xfffff,%ecx
  105dad:	8d 76 00             	lea    0x0(%esi),%esi
  105db0:	84 c0                	test   %al,%al
  105db2:	78 04                	js     105db8 <ide_read+0xe8>
  105db4:	a8 08                	test   $0x8,%al
  105db6:	75 17                	jne    105dcf <ide_read+0xff>
  105db8:	83 e9 01             	sub    $0x1,%ecx
  105dbb:	74 05                	je     105dc2 <ide_read+0xf2>
  105dbd:	ec                   	in     (%dx),%al
  105dbe:	a8 01                	test   $0x1,%al
  105dc0:	74 ee                	je     105db0 <ide_read+0xe0>
  105dc2:	83 c4 04             	add    $0x4,%esp
  105dc5:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  105dca:	5b                   	pop    %ebx
  105dcb:	5e                   	pop    %esi
  105dcc:	5f                   	pop    %edi
  105dcd:	5d                   	pop    %ebp
  105dce:	c3                   	ret
  105dcf:	89 da                	mov    %ebx,%edx
  105dd1:	66 ed                	in     (%dx),%ax
  105dd3:	66 89 87 00 fe ff ff 	mov    %ax,-0x200(%edi)
  105dda:	8d 8f 02 fe ff ff    	lea    -0x1fe(%edi),%ecx
  105de0:	0f b7 55 02          	movzwl 0x2(%ebp),%edx
  105de4:	66 ed                	in     (%dx),%ax
  105de6:	66 89 01             	mov    %ax,(%ecx)
  105de9:	83 c1 02             	add    $0x2,%ecx
  105dec:	39 f9                	cmp    %edi,%ecx
  105dee:	75 f0                	jne    105de0 <ide_read+0x110>
  105df0:	81 c7 00 02 00 00    	add    $0x200,%edi
  105df6:	39 f7                	cmp    %esi,%edi
  105df8:	74 06                	je     105e00 <ide_read+0x130>
  105dfa:	0f b7 5d 02          	movzwl 0x2(%ebp),%ebx
  105dfe:	eb a0                	jmp    105da0 <ide_read+0xd0>
  105e00:	83 c4 04             	add    $0x4,%esp
  105e03:	31 c0                	xor    %eax,%eax
  105e05:	5b                   	pop    %ebx
  105e06:	5e                   	pop    %esi
  105e07:	5f                   	pop    %edi
  105e08:	5d                   	pop    %ebp
  105e09:	c3                   	ret
  105e0a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00105e10 <ide_write>:
  105e10:	55                   	push   %ebp
  105e11:	57                   	push   %edi
  105e12:	56                   	push   %esi
  105e13:	53                   	push   %ebx
  105e14:	83 ec 04             	sub    $0x4,%esp
  105e17:	8b 74 24 20          	mov    0x20(%esp),%esi
  105e1b:	8b 44 24 18          	mov    0x18(%esp),%eax
  105e1f:	8b 6c 24 1c          	mov    0x1c(%esp),%ebp
  105e23:	89 f3                	mov    %esi,%ebx
  105e25:	88 5c 24 03          	mov    %bl,0x3(%esp)
  105e29:	3c 03                	cmp    $0x3,%al
  105e2b:	0f 87 c6 00 00 00    	ja     105ef7 <ide_write+0xe7>
  105e31:	0f b6 c0             	movzbl %al,%eax
  105e34:	6b c0 38             	imul   $0x38,%eax,%eax
  105e37:	80 b8 20 86 11 00 00 	cmpb   $0x0,0x118620(%eax)
  105e3e:	8d 90 20 86 11 00    	lea    0x118620(%eax),%edx
  105e44:	0f 84 ad 00 00 00    	je     105ef7 <ide_write+0xe7>
  105e4a:	0f b7 88 22 86 11 00 	movzwl 0x118622(%eax),%ecx
  105e51:	89 d0                	mov    %edx,%eax
  105e53:	bf 00 00 10 00       	mov    $0x100000,%edi
  105e58:	8d 59 07             	lea    0x7(%ecx),%ebx
  105e5b:	89 da                	mov    %ebx,%edx
  105e5d:	89 c3                	mov    %eax,%ebx
  105e5f:	83 ef 01             	sub    $0x1,%edi
  105e62:	74 09                	je     105e6d <ide_write+0x5d>
  105e64:	ec                   	in     (%dx),%al
  105e65:	a8 01                	test   $0x1,%al
  105e67:	75 04                	jne    105e6d <ide_write+0x5d>
  105e69:	84 c0                	test   %al,%al
  105e6b:	78 f2                	js     105e5f <ide_write+0x4f>
  105e6d:	89 d8                	mov    %ebx,%eax
  105e6f:	89 d3                	mov    %edx,%ebx
  105e71:	89 ea                	mov    %ebp,%edx
  105e73:	0f b7 78 02          	movzwl 0x2(%eax),%edi
  105e77:	0f b6 40 06          	movzbl 0x6(%eax),%eax
  105e7b:	c1 ea 18             	shr    $0x18,%edx
  105e7e:	83 e2 0f             	and    $0xf,%edx
  105e81:	c1 e0 04             	shl    $0x4,%eax
  105e84:	09 d0                	or     %edx,%eax
  105e86:	8d 57 06             	lea    0x6(%edi),%edx
  105e89:	83 c8 e0             	or     $0xffffffe0,%eax
  105e8c:	ee                   	out    %al,(%dx)
  105e8d:	0f b6 44 24 03       	movzbl 0x3(%esp),%eax
  105e92:	8d 57 02             	lea    0x2(%edi),%edx
  105e95:	ee                   	out    %al,(%dx)
  105e96:	8d 57 03             	lea    0x3(%edi),%edx
  105e99:	89 e8                	mov    %ebp,%eax
  105e9b:	ee                   	out    %al,(%dx)
  105e9c:	89 e8                	mov    %ebp,%eax
  105e9e:	8d 57 04             	lea    0x4(%edi),%edx
  105ea1:	c1 e8 08             	shr    $0x8,%eax
  105ea4:	ee                   	out    %al,(%dx)
  105ea5:	89 e8                	mov    %ebp,%eax
  105ea7:	8d 57 05             	lea    0x5(%edi),%edx
  105eaa:	c1 e8 10             	shr    $0x10,%eax
  105ead:	ee                   	out    %al,(%dx)
  105eae:	b8 30 00 00 00       	mov    $0x30,%eax
  105eb3:	89 da                	mov    %ebx,%edx
  105eb5:	ee                   	out    %al,(%dx)
  105eb6:	89 f0                	mov    %esi,%eax
  105eb8:	84 c0                	test   %al,%al
  105eba:	74 6c                	je     105f28 <ide_write+0x118>
  105ebc:	8b 44 24 24          	mov    0x24(%esp),%eax
  105ec0:	8d b8 00 02 00 00    	lea    0x200(%eax),%edi
  105ec6:	89 f0                	mov    %esi,%eax
  105ec8:	0f b6 e8             	movzbl %al,%ebp
  105ecb:	c1 e5 09             	shl    $0x9,%ebp
  105ece:	01 fd                	add    %edi,%ebp
  105ed0:	89 da                	mov    %ebx,%edx
  105ed2:	ec                   	in     (%dx),%al
  105ed3:	89 c2                	mov    %eax,%edx
  105ed5:	a8 01                	test   $0x1,%al
  105ed7:	75 1e                	jne    105ef7 <ide_write+0xe7>
  105ed9:	be ff ff 0f 00       	mov    $0xfffff,%esi
  105ede:	66 90                	xchg   %ax,%ax
  105ee0:	84 d2                	test   %dl,%dl
  105ee2:	78 05                	js     105ee9 <ide_write+0xd9>
  105ee4:	83 e2 08             	and    $0x8,%edx
  105ee7:	75 1b                	jne    105f04 <ide_write+0xf4>
  105ee9:	83 ee 01             	sub    $0x1,%esi
  105eec:	74 09                	je     105ef7 <ide_write+0xe7>
  105eee:	89 da                	mov    %ebx,%edx
  105ef0:	ec                   	in     (%dx),%al
  105ef1:	89 c2                	mov    %eax,%edx
  105ef3:	a8 01                	test   $0x1,%al
  105ef5:	74 e9                	je     105ee0 <ide_write+0xd0>
  105ef7:	83 c4 04             	add    $0x4,%esp
  105efa:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  105eff:	5b                   	pop    %ebx
  105f00:	5e                   	pop    %esi
  105f01:	5f                   	pop    %edi
  105f02:	5d                   	pop    %ebp
  105f03:	c3                   	ret
  105f04:	8d b7 00 fe ff ff    	lea    -0x200(%edi),%esi
  105f0a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  105f10:	0f b7 06             	movzwl (%esi),%eax
  105f13:	89 ca                	mov    %ecx,%edx
  105f15:	66 ef                	out    %ax,(%dx)
  105f17:	83 c6 02             	add    $0x2,%esi
  105f1a:	39 fe                	cmp    %edi,%esi
  105f1c:	75 f2                	jne    105f10 <ide_write+0x100>
  105f1e:	81 c7 00 02 00 00    	add    $0x200,%edi
  105f24:	39 ef                	cmp    %ebp,%edi
  105f26:	75 a8                	jne    105ed0 <ide_write+0xc0>
  105f28:	b8 e7 ff ff ff       	mov    $0xffffffe7,%eax
  105f2d:	89 da                	mov    %ebx,%edx
  105f2f:	ee                   	out    %al,(%dx)
  105f30:	b9 00 00 10 00       	mov    $0x100000,%ecx
  105f35:	83 e9 01             	sub    $0x1,%ecx
  105f38:	74 09                	je     105f43 <ide_write+0x133>
  105f3a:	ec                   	in     (%dx),%al
  105f3b:	a8 01                	test   $0x1,%al
  105f3d:	75 04                	jne    105f43 <ide_write+0x133>
  105f3f:	84 c0                	test   %al,%al
  105f41:	78 f2                	js     105f35 <ide_write+0x125>
  105f43:	83 c4 04             	add    $0x4,%esp
  105f46:	31 c0                	xor    %eax,%eax
  105f48:	5b                   	pop    %ebx
  105f49:	5e                   	pop    %esi
  105f4a:	5f                   	pop    %edi
  105f4b:	5d                   	pop    %ebp
  105f4c:	c3                   	ret
  105f4d:	8d 76 00             	lea    0x0(%esi),%esi

00105f50 <ide_get_drives>:
  105f50:	b8 20 86 11 00       	mov    $0x118620,%eax
  105f55:	c3                   	ret
  105f56:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  105f5d:	00 
  105f5e:	66 90                	xchg   %ax,%ax

00105f60 <ide_drive_count>:
  105f60:	a1 00 86 11 00       	mov    0x118600,%eax
  105f65:	c3                   	ret
  105f66:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  105f6d:	00 
  105f6e:	66 90                	xchg   %ax,%ax

00105f70 <ide_dump>:
  105f70:	55                   	push   %ebp
  105f71:	bd a4 94 10 00       	mov    $0x1094a4,%ebp
  105f76:	57                   	push   %edi
  105f77:	bf 9d 94 10 00       	mov    $0x10949d,%edi
  105f7c:	56                   	push   %esi
  105f7d:	31 f6                	xor    %esi,%esi
  105f7f:	53                   	push   %ebx
  105f80:	bb 2c 86 11 00       	mov    $0x11862c,%ebx
  105f85:	83 ec 0c             	sub    $0xc,%esp
  105f88:	80 7b f4 00          	cmpb   $0x0,-0xc(%ebx)
  105f8c:	74 26                	je     105fb4 <ide_dump+0x44>
  105f8e:	80 7b fa 00          	cmpb   $0x0,-0x6(%ebx)
  105f92:	89 f8                	mov    %edi,%eax
  105f94:	0f b6 53 fb          	movzbl -0x5(%ebx),%edx
  105f98:	0f 45 c5             	cmovne %ebp,%eax
  105f9b:	83 ec 04             	sub    $0x4,%esp
  105f9e:	53                   	push   %ebx
  105f9f:	ff 73 fc             	push   -0x4(%ebx)
  105fa2:	52                   	push   %edx
  105fa3:	50                   	push   %eax
  105fa4:	56                   	push   %esi
  105fa5:	68 c8 9d 10 00       	push   $0x109dc8
  105faa:	6a 01                	push   $0x1
  105fac:	e8 7f eb ff ff       	call   104b30 <klog>
  105fb1:	83 c4 20             	add    $0x20,%esp
  105fb4:	83 c6 01             	add    $0x1,%esi
  105fb7:	83 c3 38             	add    $0x38,%ebx
  105fba:	83 fe 04             	cmp    $0x4,%esi
  105fbd:	75 c9                	jne    105f88 <ide_dump+0x18>
  105fbf:	83 c4 0c             	add    $0xc,%esp
  105fc2:	5b                   	pop    %ebx
  105fc3:	5e                   	pop    %esi
  105fc4:	5f                   	pop    %edi
  105fc5:	5d                   	pop    %ebp
  105fc6:	c3                   	ret
  105fc7:	66 90                	xchg   %ax,%ax
  105fc9:	66 90                	xchg   %ax,%ax
  105fcb:	66 90                	xchg   %ax,%ax
  105fcd:	66 90                	xchg   %ax,%ax
  105fcf:	90                   	nop

00105fd0 <idle_task_fn>:
  105fd0:	fb                   	sti
  105fd1:	f4                   	hlt
  105fd2:	eb fc                	jmp    105fd0 <idle_task_fn>
  105fd4:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  105fdb:	00 
  105fdc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00105fe0 <timer_handler>:
  105fe0:	a1 00 87 11 00       	mov    0x118700,%eax
  105fe5:	57                   	push   %edi
  105fe6:	56                   	push   %esi
  105fe7:	53                   	push   %ebx
  105fe8:	8b 1d 04 87 11 00    	mov    0x118704,%ebx
  105fee:	83 c0 01             	add    $0x1,%eax
  105ff1:	a3 00 87 11 00       	mov    %eax,0x118700
  105ff6:	85 db                	test   %ebx,%ebx
  105ff8:	0f 84 ca 00 00 00    	je     1060c8 <timer_handler+0xe8>
  105ffe:	8d 14 80             	lea    (%eax,%eax,4),%edx
  106001:	b8 24 87 11 00       	mov    $0x118724,%eax
  106006:	01 d2                	add    %edx,%edx
  106008:	eb 10                	jmp    10601a <timer_handler+0x3a>
  10600a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  106010:	83 c0 60             	add    $0x60,%eax
  106013:	3d 24 9f 11 00       	cmp    $0x119f24,%eax
  106018:	74 1e                	je     106038 <timer_handler+0x58>
  10601a:	83 38 03             	cmpl   $0x3,(%eax)
  10601d:	75 f1                	jne    106010 <timer_handler+0x30>
  10601f:	3b 50 34             	cmp    0x34(%eax),%edx
  106022:	72 ec                	jb     106010 <timer_handler+0x30>
  106024:	c7 00 01 00 00 00    	movl   $0x1,(%eax)
  10602a:	83 c0 60             	add    $0x60,%eax
  10602d:	3d 24 9f 11 00       	cmp    $0x119f24,%eax
  106032:	75 e6                	jne    10601a <timer_handler+0x3a>
  106034:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  106038:	89 de                	mov    %ebx,%esi
  10603a:	81 ee 20 87 11 00    	sub    $0x118720,%esi
  106040:	c1 fe 05             	sar    $0x5,%esi
  106043:	69 f6 ab aa aa aa    	imul   $0xaaaaaaab,%esi,%esi
  106049:	8d 4e 01             	lea    0x1(%esi),%ecx
  10604c:	83 c6 41             	add    $0x41,%esi
  10604f:	eb 0e                	jmp    10605f <timer_handler+0x7f>
  106051:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  106058:	83 c1 01             	add    $0x1,%ecx
  10605b:	39 f1                	cmp    %esi,%ecx
  10605d:	74 69                	je     1060c8 <timer_handler+0xe8>
  10605f:	89 ca                	mov    %ecx,%edx
  106061:	c1 fa 1f             	sar    $0x1f,%edx
  106064:	c1 ea 1a             	shr    $0x1a,%edx
  106067:	8d 04 11             	lea    (%ecx,%edx,1),%eax
  10606a:	83 e0 3f             	and    $0x3f,%eax
  10606d:	29 d0                	sub    %edx,%eax
  10606f:	8d 3c 40             	lea    (%eax,%eax,2),%edi
  106072:	c1 e7 05             	shl    $0x5,%edi
  106075:	8d 97 20 87 11 00    	lea    0x118720(%edi),%edx
  10607b:	8b 42 04             	mov    0x4(%edx),%eax
  10607e:	83 e8 01             	sub    $0x1,%eax
  106081:	83 f8 01             	cmp    $0x1,%eax
  106084:	77 d2                	ja     106058 <timer_handler+0x78>
  106086:	39 d3                	cmp    %edx,%ebx
  106088:	74 3e                	je     1060c8 <timer_handler+0xe8>
  10608a:	8b 43 04             	mov    0x4(%ebx),%eax
  10608d:	b9 01 00 00 00       	mov    $0x1,%ecx
  106092:	89 15 04 87 11 00    	mov    %edx,0x118704
  106098:	83 f8 02             	cmp    $0x2,%eax
  10609b:	0f 44 c1             	cmove  %ecx,%eax
  10609e:	83 ec 0c             	sub    $0xc,%esp
  1060a1:	81 c7 28 87 11 00    	add    $0x118728,%edi
  1060a7:	83 c3 08             	add    $0x8,%ebx
  1060aa:	89 43 fc             	mov    %eax,-0x4(%ebx)
  1060ad:	c7 42 04 02 00 00 00 	movl   $0x2,0x4(%edx)
  1060b4:	ff 72 30             	push   0x30(%edx)
  1060b7:	e8 44 c0 ff ff       	call   102100 <tss_set_kernel_stack>
  1060bc:	58                   	pop    %eax
  1060bd:	5a                   	pop    %edx
  1060be:	57                   	push   %edi
  1060bf:	53                   	push   %ebx
  1060c0:	e8 7b af ff ff       	call   101040 <sched_switch_context>
  1060c5:	83 c4 10             	add    $0x10,%esp
  1060c8:	5b                   	pop    %ebx
  1060c9:	5e                   	pop    %esi
  1060ca:	5f                   	pop    %edi
  1060cb:	c3                   	ret
  1060cc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

001060d0 <task_create>:
  1060d0:	55                   	push   %ebp
  1060d1:	b8 24 87 11 00       	mov    $0x118724,%eax
  1060d6:	57                   	push   %edi
  1060d7:	56                   	push   %esi
  1060d8:	53                   	push   %ebx
  1060d9:	31 db                	xor    %ebx,%ebx
  1060db:	83 ec 0c             	sub    $0xc,%esp
  1060de:	8b 74 24 24          	mov    0x24(%esp),%esi
  1060e2:	eb 13                	jmp    1060f7 <task_create+0x27>
  1060e4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  1060e8:	83 c3 01             	add    $0x1,%ebx
  1060eb:	83 c0 60             	add    $0x60,%eax
  1060ee:	83 fb 40             	cmp    $0x40,%ebx
  1060f1:	0f 84 c1 00 00 00    	je     1061b8 <task_create+0xe8>
  1060f7:	8b 10                	mov    (%eax),%edx
  1060f9:	85 d2                	test   %edx,%edx
  1060fb:	75 eb                	jne    1060e8 <task_create+0x18>
  1060fd:	83 ec 0c             	sub    $0xc,%esp
  106100:	6a 02                	push   $0x2
  106102:	e8 19 ce ff ff       	call   102f20 <pmm_alloc_pages>
  106107:	83 c4 10             	add    $0x10,%esp
  10610a:	89 c5                	mov    %eax,%ebp
  10610c:	85 c0                	test   %eax,%eax
  10610e:	0f 84 a4 00 00 00    	je     1061b8 <task_create+0xe8>
  106114:	8d 14 5b             	lea    (%ebx,%ebx,2),%edx
  106117:	83 ec 04             	sub    $0x4,%esp
  10611a:	c1 e2 05             	shl    $0x5,%edx
  10611d:	6a 60                	push   $0x60
  10611f:	8d 9a 20 87 11 00    	lea    0x118720(%edx),%ebx
  106125:	6a 00                	push   $0x0
  106127:	89 d7                	mov    %edx,%edi
  106129:	53                   	push   %ebx
  10612a:	e8 31 b4 ff ff       	call   101560 <memset>
  10612f:	a1 04 b0 10 00       	mov    0x10b004,%eax
  106134:	83 c4 0c             	add    $0xc,%esp
  106137:	89 af 54 87 11 00    	mov    %ebp,0x118754(%edi)
  10613d:	81 c5 00 20 00 00    	add    $0x2000,%ebp
  106143:	85 f6                	test   %esi,%esi
  106145:	6a 1f                	push   $0x1f
  106147:	8d 97 5c 87 11 00    	lea    0x11875c(%edi),%edx
  10614d:	8d 48 01             	lea    0x1(%eax),%ecx
  106150:	89 87 20 87 11 00    	mov    %eax,0x118720(%edi)
  106156:	b8 aa 94 10 00       	mov    $0x1094aa,%eax
  10615b:	0f 44 f0             	cmove  %eax,%esi
  10615e:	89 af 50 87 11 00    	mov    %ebp,0x118750(%edi)
  106164:	89 0d 04 b0 10 00    	mov    %ecx,0x10b004
  10616a:	56                   	push   %esi
  10616b:	52                   	push   %edx
  10616c:	c7 87 24 87 11 00 01 	movl   $0x1,0x118724(%edi)
  106173:	00 00 00 
  106176:	e8 75 b5 ff ff       	call   1016f0 <strncpy>
  10617b:	8b 87 50 87 11 00    	mov    0x118750(%edi),%eax
  106181:	8b 4c 24 30          	mov    0x30(%esp),%ecx
  106185:	83 c4 10             	add    $0x10,%esp
  106188:	c7 40 fc 00 00 00 00 	movl   $0x0,-0x4(%eax)
  10618f:	83 e8 08             	sub    $0x8,%eax
  106192:	89 08                	mov    %ecx,(%eax)
  106194:	89 87 34 87 11 00    	mov    %eax,0x118734(%edi)
  10619a:	89 d8                	mov    %ebx,%eax
  10619c:	89 8f 48 87 11 00    	mov    %ecx,0x118748(%edi)
  1061a2:	c7 87 4c 87 11 00 02 	movl   $0x202,0x11874c(%edi)
  1061a9:	02 00 00 
  1061ac:	83 c4 0c             	add    $0xc,%esp
  1061af:	5b                   	pop    %ebx
  1061b0:	5e                   	pop    %esi
  1061b1:	5f                   	pop    %edi
  1061b2:	5d                   	pop    %ebp
  1061b3:	c3                   	ret
  1061b4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  1061b8:	83 c4 0c             	add    $0xc,%esp
  1061bb:	31 db                	xor    %ebx,%ebx
  1061bd:	89 d8                	mov    %ebx,%eax
  1061bf:	5b                   	pop    %ebx
  1061c0:	5e                   	pop    %esi
  1061c1:	5f                   	pop    %edi
  1061c2:	5d                   	pop    %ebp
  1061c3:	c3                   	ret
  1061c4:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1061cb:	00 
  1061cc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

001061d0 <sched_init>:
  1061d0:	83 ec 10             	sub    $0x10,%esp
  1061d3:	68 00 18 00 00       	push   $0x1800
  1061d8:	6a 00                	push   $0x0
  1061da:	68 20 87 11 00       	push   $0x118720
  1061df:	e8 7c b3 ff ff       	call   101560 <memset>
  1061e4:	b8 36 00 00 00       	mov    $0x36,%eax
  1061e9:	e6 43                	out    %al,$0x43
  1061eb:	b8 9b ff ff ff       	mov    $0xffffff9b,%eax
  1061f0:	e6 40                	out    %al,$0x40
  1061f2:	b8 2e 00 00 00       	mov    $0x2e,%eax
  1061f7:	e6 40                	out    %al,$0x40
  1061f9:	58                   	pop    %eax
  1061fa:	5a                   	pop    %edx
  1061fb:	68 e0 5f 10 00       	push   $0x105fe0
  106200:	6a 00                	push   $0x0
  106202:	e8 89 c5 ff ff       	call   102790 <irq_register>
  106207:	59                   	pop    %ecx
  106208:	58                   	pop    %eax
  106209:	68 b2 94 10 00       	push   $0x1094b2
  10620e:	68 d0 5f 10 00       	push   $0x105fd0
  106213:	e8 b8 fe ff ff       	call   1060d0 <task_create>
  106218:	83 c4 10             	add    $0x10,%esp
  10621b:	85 c0                	test   %eax,%eax
  10621d:	74 2c                	je     10624b <sched_init+0x7b>
  10621f:	c7 40 04 02 00 00 00 	movl   $0x2,0x4(%eax)
  106226:	83 ec 0c             	sub    $0xc,%esp
  106229:	ff 70 30             	push   0x30(%eax)
  10622c:	a3 04 87 11 00       	mov    %eax,0x118704
  106231:	e8 ca be ff ff       	call   102100 <tss_set_kernel_stack>
  106236:	83 c4 0c             	add    $0xc,%esp
  106239:	6a 64                	push   $0x64
  10623b:	68 18 9e 10 00       	push   $0x109e18
  106240:	6a 01                	push   $0x1
  106242:	e8 e9 e8 ff ff       	call   104b30 <klog>
  106247:	83 c4 1c             	add    $0x1c,%esp
  10624a:	c3                   	ret
  10624b:	83 ec 0c             	sub    $0xc,%esp
  10624e:	68 f0 9d 10 00       	push   $0x109df0
  106253:	e8 b8 c7 ff ff       	call   102a10 <kpanic>
  106258:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10625f:	00 

00106260 <task_exit>:
  106260:	53                   	push   %ebx
  106261:	83 ec 08             	sub    $0x8,%esp
  106264:	9c                   	pushf
  106265:	5b                   	pop    %ebx
  106266:	fa                   	cli
  106267:	a1 04 87 11 00       	mov    0x118704,%eax
  10626c:	85 c0                	test   %eax,%eax
  10626e:	74 1d                	je     10628d <task_exit+0x2d>
  106270:	8b 54 24 10          	mov    0x10(%esp),%edx
  106274:	c7 40 04 04 00 00 00 	movl   $0x4,0x4(%eax)
  10627b:	89 50 5c             	mov    %edx,0x5c(%eax)
  10627e:	52                   	push   %edx
  10627f:	52                   	push   %edx
  106280:	6a 02                	push   $0x2
  106282:	ff 70 34             	push   0x34(%eax)
  106285:	e8 86 cd ff ff       	call   103010 <pmm_free_pages>
  10628a:	83 c4 10             	add    $0x10,%esp
  10628d:	53                   	push   %ebx
  10628e:	9d                   	popf
  10628f:	cd 20                	int    $0x20
  106291:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  106298:	f4                   	hlt
  106299:	eb fd                	jmp    106298 <task_exit+0x38>
  10629b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

001062a0 <task_yield>:
  1062a0:	cd 20                	int    $0x20
  1062a2:	c3                   	ret
  1062a3:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1062aa:	00 
  1062ab:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

001062b0 <task_sleep>:
  1062b0:	a1 04 87 11 00       	mov    0x118704,%eax
  1062b5:	85 c0                	test   %eax,%eax
  1062b7:	74 27                	je     1062e0 <task_sleep+0x30>
  1062b9:	53                   	push   %ebx
  1062ba:	9c                   	pushf
  1062bb:	59                   	pop    %ecx
  1062bc:	fa                   	cli
  1062bd:	8b 15 00 87 11 00    	mov    0x118700,%edx
  1062c3:	c7 40 04 03 00 00 00 	movl   $0x3,0x4(%eax)
  1062ca:	8d 1c 92             	lea    (%edx,%edx,4),%ebx
  1062cd:	8b 54 24 08          	mov    0x8(%esp),%edx
  1062d1:	8d 14 5a             	lea    (%edx,%ebx,2),%edx
  1062d4:	89 50 38             	mov    %edx,0x38(%eax)
  1062d7:	51                   	push   %ecx
  1062d8:	9d                   	popf
  1062d9:	cd 20                	int    $0x20
  1062db:	5b                   	pop    %ebx
  1062dc:	c3                   	ret
  1062dd:	8d 76 00             	lea    0x0(%esi),%esi
  1062e0:	c3                   	ret
  1062e1:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1062e8:	00 
  1062e9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

001062f0 <task_wake>:
  1062f0:	8b 44 24 04          	mov    0x4(%esp),%eax
  1062f4:	85 c0                	test   %eax,%eax
  1062f6:	74 06                	je     1062fe <task_wake+0xe>
  1062f8:	83 78 04 03          	cmpl   $0x3,0x4(%eax)
  1062fc:	74 02                	je     106300 <task_wake+0x10>
  1062fe:	c3                   	ret
  1062ff:	90                   	nop
  106300:	c7 40 38 00 00 00 00 	movl   $0x0,0x38(%eax)
  106307:	c7 40 04 01 00 00 00 	movl   $0x1,0x4(%eax)
  10630e:	c3                   	ret
  10630f:	90                   	nop

00106310 <sched_current>:
  106310:	a1 04 87 11 00       	mov    0x118704,%eax
  106315:	c3                   	ret
  106316:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10631d:	00 
  10631e:	66 90                	xchg   %ax,%ax

00106320 <sched_uptime_ms>:
  106320:	a1 00 87 11 00       	mov    0x118700,%eax
  106325:	8d 04 80             	lea    (%eax,%eax,4),%eax
  106328:	01 c0                	add    %eax,%eax
  10632a:	c3                   	ret
  10632b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00106330 <sched_task_count>:
  106330:	b8 24 87 11 00       	mov    $0x118724,%eax
  106335:	31 c9                	xor    %ecx,%ecx
  106337:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10633e:	00 
  10633f:	90                   	nop
  106340:	8b 10                	mov    (%eax),%edx
  106342:	83 ea 01             	sub    $0x1,%edx
  106345:	83 fa 02             	cmp    $0x2,%edx
  106348:	83 d1 00             	adc    $0x0,%ecx
  10634b:	83 c0 60             	add    $0x60,%eax
  10634e:	3d 24 9f 11 00       	cmp    $0x119f24,%eax
  106353:	75 eb                	jne    106340 <sched_task_count+0x10>
  106355:	89 c8                	mov    %ecx,%eax
  106357:	c3                   	ret
  106358:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10635f:	00 

00106360 <sched_dump>:
  106360:	53                   	push   %ebx
  106361:	bb 5c 87 11 00       	mov    $0x11875c,%ebx
  106366:	83 ec 0c             	sub    $0xc,%esp
  106369:	a1 00 87 11 00       	mov    0x118700,%eax
  10636e:	8d 04 80             	lea    (%eax,%eax,4),%eax
  106371:	01 c0                	add    %eax,%eax
  106373:	50                   	push   %eax
  106374:	68 b7 94 10 00       	push   $0x1094b7
  106379:	6a 01                	push   $0x1
  10637b:	e8 b0 e7 ff ff       	call   104b30 <klog>
  106380:	83 c4 10             	add    $0x10,%esp
  106383:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  106388:	8b 43 c8             	mov    -0x38(%ebx),%eax
  10638b:	85 c0                	test   %eax,%eax
  10638d:	74 20                	je     1063af <sched_dump+0x4f>
  10638f:	83 ec 08             	sub    $0x8,%esp
  106392:	ff 73 d8             	push   -0x28(%ebx)
  106395:	ff 34 85 58 ac 10 00 	push   0x10ac58(,%eax,4)
  10639c:	53                   	push   %ebx
  10639d:	ff 73 c4             	push   -0x3c(%ebx)
  1063a0:	68 40 9e 10 00       	push   $0x109e40
  1063a5:	6a 01                	push   $0x1
  1063a7:	e8 84 e7 ff ff       	call   104b30 <klog>
  1063ac:	83 c4 20             	add    $0x20,%esp
  1063af:	83 c3 60             	add    $0x60,%ebx
  1063b2:	81 fb 5c 9f 11 00    	cmp    $0x119f5c,%ebx
  1063b8:	75 ce                	jne    106388 <sched_dump+0x28>
  1063ba:	83 c4 08             	add    $0x8,%esp
  1063bd:	5b                   	pop    %ebx
  1063be:	c3                   	ret
  1063bf:	90                   	nop

001063c0 <sem_init>:
  1063c0:	83 ec 10             	sub    $0x10,%esp
  1063c3:	8b 44 24 14          	mov    0x14(%esp),%eax
  1063c7:	8b 54 24 18          	mov    0x18(%esp),%edx
  1063cb:	c7 40 48 00 00 00 00 	movl   $0x0,0x48(%eax)
  1063d2:	83 c0 08             	add    $0x8,%eax
  1063d5:	89 50 f8             	mov    %edx,-0x8(%eax)
  1063d8:	c7 40 fc 00 00 00 00 	movl   $0x0,-0x4(%eax)
  1063df:	6a 40                	push   $0x40
  1063e1:	6a 00                	push   $0x0
  1063e3:	50                   	push   %eax
  1063e4:	e8 77 b1 ff ff       	call   101560 <memset>
  1063e9:	83 c4 1c             	add    $0x1c,%esp
  1063ec:	c3                   	ret
  1063ed:	8d 76 00             	lea    0x0(%esi),%esi

001063f0 <sem_wait>:
  1063f0:	55                   	push   %ebp
  1063f1:	57                   	push   %edi
  1063f2:	bf 01 00 00 00       	mov    $0x1,%edi
  1063f7:	56                   	push   %esi
  1063f8:	53                   	push   %ebx
  1063f9:	83 ec 0c             	sub    $0xc,%esp
  1063fc:	8b 5c 24 20          	mov    0x20(%esp),%ebx
  106400:	8d 73 04             	lea    0x4(%ebx),%esi
  106403:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  106408:	9c                   	pushf
  106409:	5d                   	pop    %ebp
  10640a:	fa                   	cli
  10640b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  106410:	89 f8                	mov    %edi,%eax
  106412:	87 06                	xchg   %eax,(%esi)
  106414:	85 c0                	test   %eax,%eax
  106416:	74 18                	je     106430 <sem_wait+0x40>
  106418:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10641f:	00 
  106420:	8b 43 04             	mov    0x4(%ebx),%eax
  106423:	85 c0                	test   %eax,%eax
  106425:	74 e9                	je     106410 <sem_wait+0x20>
  106427:	f3 90                	pause
  106429:	eb f5                	jmp    106420 <sem_wait+0x30>
  10642b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  106430:	8b 03                	mov    (%ebx),%eax
  106432:	85 c0                	test   %eax,%eax
  106434:	7f 34                	jg     10646a <sem_wait+0x7a>
  106436:	e8 d5 fe ff ff       	call   106310 <sched_current>
  10643b:	85 c0                	test   %eax,%eax
  10643d:	74 14                	je     106453 <sem_wait+0x63>
  10643f:	8b 53 48             	mov    0x48(%ebx),%edx
  106442:	83 fa 0f             	cmp    $0xf,%edx
  106445:	77 0c                	ja     106453 <sem_wait+0x63>
  106447:	8b 00                	mov    (%eax),%eax
  106449:	8d 4a 01             	lea    0x1(%edx),%ecx
  10644c:	89 4b 48             	mov    %ecx,0x48(%ebx)
  10644f:	89 44 93 08          	mov    %eax,0x8(%ebx,%edx,4)
  106453:	c7 06 00 00 00 00    	movl   $0x0,(%esi)
  106459:	55                   	push   %ebp
  10645a:	9d                   	popf
  10645b:	83 ec 0c             	sub    $0xc,%esp
  10645e:	6a 01                	push   $0x1
  106460:	e8 4b fe ff ff       	call   1062b0 <task_sleep>
  106465:	83 c4 10             	add    $0x10,%esp
  106468:	eb 9e                	jmp    106408 <sem_wait+0x18>
  10646a:	8b 03                	mov    (%ebx),%eax
  10646c:	83 e8 01             	sub    $0x1,%eax
  10646f:	89 03                	mov    %eax,(%ebx)
  106471:	c7 43 04 00 00 00 00 	movl   $0x0,0x4(%ebx)
  106478:	55                   	push   %ebp
  106479:	9d                   	popf
  10647a:	83 c4 0c             	add    $0xc,%esp
  10647d:	5b                   	pop    %ebx
  10647e:	5e                   	pop    %esi
  10647f:	5f                   	pop    %edi
  106480:	5d                   	pop    %ebp
  106481:	c3                   	ret
  106482:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106489:	00 
  10648a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00106490 <sem_trywait>:
  106490:	56                   	push   %esi
  106491:	53                   	push   %ebx
  106492:	8b 54 24 0c          	mov    0xc(%esp),%edx
  106496:	9c                   	pushf
  106497:	5e                   	pop    %esi
  106498:	fa                   	cli
  106499:	bb 01 00 00 00       	mov    $0x1,%ebx
  10649e:	8d 4a 04             	lea    0x4(%edx),%ecx
  1064a1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  1064a8:	89 d8                	mov    %ebx,%eax
  1064aa:	87 01                	xchg   %eax,(%ecx)
  1064ac:	85 c0                	test   %eax,%eax
  1064ae:	74 10                	je     1064c0 <sem_trywait+0x30>
  1064b0:	8b 42 04             	mov    0x4(%edx),%eax
  1064b3:	85 c0                	test   %eax,%eax
  1064b5:	74 f1                	je     1064a8 <sem_trywait+0x18>
  1064b7:	f3 90                	pause
  1064b9:	eb f5                	jmp    1064b0 <sem_trywait+0x20>
  1064bb:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  1064c0:	8b 02                	mov    (%edx),%eax
  1064c2:	85 c0                	test   %eax,%eax
  1064c4:	0f 9f c1             	setg   %cl
  1064c7:	7e 07                	jle    1064d0 <sem_trywait+0x40>
  1064c9:	8b 02                	mov    (%edx),%eax
  1064cb:	83 e8 01             	sub    $0x1,%eax
  1064ce:	89 02                	mov    %eax,(%edx)
  1064d0:	c7 42 04 00 00 00 00 	movl   $0x0,0x4(%edx)
  1064d7:	56                   	push   %esi
  1064d8:	9d                   	popf
  1064d9:	89 c8                	mov    %ecx,%eax
  1064db:	5b                   	pop    %ebx
  1064dc:	5e                   	pop    %esi
  1064dd:	c3                   	ret
  1064de:	66 90                	xchg   %ax,%ax

001064e0 <sem_post>:
  1064e0:	56                   	push   %esi
  1064e1:	53                   	push   %ebx
  1064e2:	8b 54 24 0c          	mov    0xc(%esp),%edx
  1064e6:	9c                   	pushf
  1064e7:	5e                   	pop    %esi
  1064e8:	fa                   	cli
  1064e9:	bb 01 00 00 00       	mov    $0x1,%ebx
  1064ee:	8d 4a 04             	lea    0x4(%edx),%ecx
  1064f1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  1064f8:	89 d8                	mov    %ebx,%eax
  1064fa:	87 01                	xchg   %eax,(%ecx)
  1064fc:	85 c0                	test   %eax,%eax
  1064fe:	74 10                	je     106510 <sem_post+0x30>
  106500:	8b 42 04             	mov    0x4(%edx),%eax
  106503:	85 c0                	test   %eax,%eax
  106505:	74 f1                	je     1064f8 <sem_post+0x18>
  106507:	f3 90                	pause
  106509:	eb f5                	jmp    106500 <sem_post+0x20>
  10650b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  106510:	8b 02                	mov    (%edx),%eax
  106512:	83 c0 01             	add    $0x1,%eax
  106515:	89 02                	mov    %eax,(%edx)
  106517:	8b 42 48             	mov    0x48(%edx),%eax
  10651a:	85 c0                	test   %eax,%eax
  10651c:	74 06                	je     106524 <sem_post+0x44>
  10651e:	83 e8 01             	sub    $0x1,%eax
  106521:	89 42 48             	mov    %eax,0x48(%edx)
  106524:	c7 42 04 00 00 00 00 	movl   $0x0,0x4(%edx)
  10652b:	56                   	push   %esi
  10652c:	9d                   	popf
  10652d:	5b                   	pop    %ebx
  10652e:	5e                   	pop    %esi
  10652f:	c3                   	ret

00106530 <mutex_init>:
  106530:	83 ec 10             	sub    $0x10,%esp
  106533:	8b 44 24 14          	mov    0x14(%esp),%eax
  106537:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
  10653d:	83 c0 0c             	add    $0xc,%eax
  106540:	c7 40 f8 00 00 00 00 	movl   $0x0,-0x8(%eax)
  106547:	c7 40 40 00 00 00 00 	movl   $0x0,0x40(%eax)
  10654e:	c7 40 fc 00 00 00 00 	movl   $0x0,-0x4(%eax)
  106555:	6a 40                	push   $0x40
  106557:	6a 00                	push   $0x0
  106559:	50                   	push   %eax
  10655a:	e8 01 b0 ff ff       	call   101560 <memset>
  10655f:	83 c4 1c             	add    $0x1c,%esp
  106562:	c3                   	ret
  106563:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10656a:	00 
  10656b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00106570 <mutex_lock>:
  106570:	55                   	push   %ebp
  106571:	31 ed                	xor    %ebp,%ebp
  106573:	57                   	push   %edi
  106574:	56                   	push   %esi
  106575:	53                   	push   %ebx
  106576:	83 ec 0c             	sub    $0xc,%esp
  106579:	8b 5c 24 20          	mov    0x20(%esp),%ebx
  10657d:	e8 8e fd ff ff       	call   106310 <sched_current>
  106582:	85 c0                	test   %eax,%eax
  106584:	74 02                	je     106588 <mutex_lock+0x18>
  106586:	8b 28                	mov    (%eax),%ebp
  106588:	8d 73 08             	lea    0x8(%ebx),%esi
  10658b:	bf 01 00 00 00       	mov    $0x1,%edi
  106590:	9c                   	pushf
  106591:	5a                   	pop    %edx
  106592:	fa                   	cli
  106593:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  106598:	89 f8                	mov    %edi,%eax
  10659a:	87 06                	xchg   %eax,(%esi)
  10659c:	85 c0                	test   %eax,%eax
  10659e:	74 10                	je     1065b0 <mutex_lock+0x40>
  1065a0:	8b 43 08             	mov    0x8(%ebx),%eax
  1065a3:	85 c0                	test   %eax,%eax
  1065a5:	74 f1                	je     106598 <mutex_lock+0x28>
  1065a7:	f3 90                	pause
  1065a9:	eb f5                	jmp    1065a0 <mutex_lock+0x30>
  1065ab:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  1065b0:	8b 03                	mov    (%ebx),%eax
  1065b2:	39 e8                	cmp    %ebp,%eax
  1065b4:	74 2b                	je     1065e1 <mutex_lock+0x71>
  1065b6:	8b 03                	mov    (%ebx),%eax
  1065b8:	85 c0                	test   %eax,%eax
  1065ba:	74 3a                	je     1065f6 <mutex_lock+0x86>
  1065bc:	8b 43 4c             	mov    0x4c(%ebx),%eax
  1065bf:	85 ed                	test   %ebp,%ebp
  1065c1:	74 0f                	je     1065d2 <mutex_lock+0x62>
  1065c3:	83 f8 0f             	cmp    $0xf,%eax
  1065c6:	77 0a                	ja     1065d2 <mutex_lock+0x62>
  1065c8:	8d 48 01             	lea    0x1(%eax),%ecx
  1065cb:	89 4b 4c             	mov    %ecx,0x4c(%ebx)
  1065ce:	89 6c 83 0c          	mov    %ebp,0xc(%ebx,%eax,4)
  1065d2:	c7 06 00 00 00 00    	movl   $0x0,(%esi)
  1065d8:	52                   	push   %edx
  1065d9:	9d                   	popf
  1065da:	e8 c1 fc ff ff       	call   1062a0 <task_yield>
  1065df:	eb af                	jmp    106590 <mutex_lock+0x20>
  1065e1:	83 43 04 01          	addl   $0x1,0x4(%ebx)
  1065e5:	c7 43 08 00 00 00 00 	movl   $0x0,0x8(%ebx)
  1065ec:	52                   	push   %edx
  1065ed:	9d                   	popf
  1065ee:	83 c4 0c             	add    $0xc,%esp
  1065f1:	5b                   	pop    %ebx
  1065f2:	5e                   	pop    %esi
  1065f3:	5f                   	pop    %edi
  1065f4:	5d                   	pop    %ebp
  1065f5:	c3                   	ret
  1065f6:	89 2b                	mov    %ebp,(%ebx)
  1065f8:	c7 43 04 01 00 00 00 	movl   $0x1,0x4(%ebx)
  1065ff:	c7 43 08 00 00 00 00 	movl   $0x0,0x8(%ebx)
  106606:	52                   	push   %edx
  106607:	9d                   	popf
  106608:	83 c4 0c             	add    $0xc,%esp
  10660b:	5b                   	pop    %ebx
  10660c:	5e                   	pop    %esi
  10660d:	5f                   	pop    %edi
  10660e:	5d                   	pop    %ebp
  10660f:	c3                   	ret

00106610 <mutex_trylock>:
  106610:	57                   	push   %edi
  106611:	56                   	push   %esi
  106612:	31 f6                	xor    %esi,%esi
  106614:	53                   	push   %ebx
  106615:	8b 5c 24 10          	mov    0x10(%esp),%ebx
  106619:	e8 f2 fc ff ff       	call   106310 <sched_current>
  10661e:	85 c0                	test   %eax,%eax
  106620:	74 02                	je     106624 <mutex_trylock+0x14>
  106622:	8b 30                	mov    (%eax),%esi
  106624:	9c                   	pushf
  106625:	5f                   	pop    %edi
  106626:	fa                   	cli
  106627:	b9 01 00 00 00       	mov    $0x1,%ecx
  10662c:	8d 53 08             	lea    0x8(%ebx),%edx
  10662f:	90                   	nop
  106630:	89 c8                	mov    %ecx,%eax
  106632:	87 02                	xchg   %eax,(%edx)
  106634:	85 c0                	test   %eax,%eax
  106636:	74 18                	je     106650 <mutex_trylock+0x40>
  106638:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10663f:	00 
  106640:	8b 43 08             	mov    0x8(%ebx),%eax
  106643:	85 c0                	test   %eax,%eax
  106645:	74 e9                	je     106630 <mutex_trylock+0x20>
  106647:	f3 90                	pause
  106649:	eb f5                	jmp    106640 <mutex_trylock+0x30>
  10664b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  106650:	8b 03                	mov    (%ebx),%eax
  106652:	85 c0                	test   %eax,%eax
  106654:	74 15                	je     10666b <mutex_trylock+0x5b>
  106656:	8b 13                	mov    (%ebx),%edx
  106658:	31 c0                	xor    %eax,%eax
  10665a:	39 f2                	cmp    %esi,%edx
  10665c:	74 0d                	je     10666b <mutex_trylock+0x5b>
  10665e:	c7 43 08 00 00 00 00 	movl   $0x0,0x8(%ebx)
  106665:	57                   	push   %edi
  106666:	9d                   	popf
  106667:	5b                   	pop    %ebx
  106668:	5e                   	pop    %esi
  106669:	5f                   	pop    %edi
  10666a:	c3                   	ret
  10666b:	83 43 04 01          	addl   $0x1,0x4(%ebx)
  10666f:	b8 01 00 00 00       	mov    $0x1,%eax
  106674:	89 33                	mov    %esi,(%ebx)
  106676:	eb e6                	jmp    10665e <mutex_trylock+0x4e>
  106678:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10667f:	00 

00106680 <mutex_unlock>:
  106680:	56                   	push   %esi
  106681:	53                   	push   %ebx
  106682:	8b 54 24 0c          	mov    0xc(%esp),%edx
  106686:	9c                   	pushf
  106687:	5e                   	pop    %esi
  106688:	fa                   	cli
  106689:	bb 01 00 00 00       	mov    $0x1,%ebx
  10668e:	8d 4a 08             	lea    0x8(%edx),%ecx
  106691:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  106698:	89 d8                	mov    %ebx,%eax
  10669a:	87 01                	xchg   %eax,(%ecx)
  10669c:	85 c0                	test   %eax,%eax
  10669e:	74 10                	je     1066b0 <mutex_unlock+0x30>
  1066a0:	8b 42 08             	mov    0x8(%edx),%eax
  1066a3:	85 c0                	test   %eax,%eax
  1066a5:	74 f1                	je     106698 <mutex_unlock+0x18>
  1066a7:	f3 90                	pause
  1066a9:	eb f5                	jmp    1066a0 <mutex_unlock+0x20>
  1066ab:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  1066b0:	8b 42 04             	mov    0x4(%edx),%eax
  1066b3:	83 f8 01             	cmp    $0x1,%eax
  1066b6:	76 12                	jbe    1066ca <mutex_unlock+0x4a>
  1066b8:	83 e8 01             	sub    $0x1,%eax
  1066bb:	89 42 04             	mov    %eax,0x4(%edx)
  1066be:	c7 42 08 00 00 00 00 	movl   $0x0,0x8(%edx)
  1066c5:	56                   	push   %esi
  1066c6:	9d                   	popf
  1066c7:	5b                   	pop    %ebx
  1066c8:	5e                   	pop    %esi
  1066c9:	c3                   	ret
  1066ca:	8b 42 4c             	mov    0x4c(%edx),%eax
  1066cd:	c7 02 00 00 00 00    	movl   $0x0,(%edx)
  1066d3:	c7 42 04 00 00 00 00 	movl   $0x0,0x4(%edx)
  1066da:	85 c0                	test   %eax,%eax
  1066dc:	74 e0                	je     1066be <mutex_unlock+0x3e>
  1066de:	83 e8 01             	sub    $0x1,%eax
  1066e1:	89 42 4c             	mov    %eax,0x4c(%edx)
  1066e4:	eb d8                	jmp    1066be <mutex_unlock+0x3e>
  1066e6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1066ed:	00 
  1066ee:	66 90                	xchg   %ax,%ax

001066f0 <condvar_init>:
  1066f0:	83 ec 10             	sub    $0x10,%esp
  1066f3:	8b 44 24 14          	mov    0x14(%esp),%eax
  1066f7:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
  1066fd:	83 c0 04             	add    $0x4,%eax
  106700:	c7 40 40 00 00 00 00 	movl   $0x0,0x40(%eax)
  106707:	6a 40                	push   $0x40
  106709:	6a 00                	push   $0x0
  10670b:	50                   	push   %eax
  10670c:	e8 4f ae ff ff       	call   101560 <memset>
  106711:	83 c4 1c             	add    $0x1c,%esp
  106714:	c3                   	ret
  106715:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10671c:	00 
  10671d:	8d 76 00             	lea    0x0(%esi),%esi

00106720 <condvar_wait>:
  106720:	57                   	push   %edi
  106721:	56                   	push   %esi
  106722:	53                   	push   %ebx
  106723:	8b 5c 24 10          	mov    0x10(%esp),%ebx
  106727:	8b 74 24 14          	mov    0x14(%esp),%esi
  10672b:	e8 e0 fb ff ff       	call   106310 <sched_current>
  106730:	89 c1                	mov    %eax,%ecx
  106732:	9c                   	pushf
  106733:	5f                   	pop    %edi
  106734:	fa                   	cli
  106735:	ba 01 00 00 00       	mov    $0x1,%edx
  10673a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  106740:	89 d0                	mov    %edx,%eax
  106742:	87 03                	xchg   %eax,(%ebx)
  106744:	85 c0                	test   %eax,%eax
  106746:	74 18                	je     106760 <condvar_wait+0x40>
  106748:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10674f:	00 
  106750:	8b 03                	mov    (%ebx),%eax
  106752:	85 c0                	test   %eax,%eax
  106754:	74 ea                	je     106740 <condvar_wait+0x20>
  106756:	f3 90                	pause
  106758:	eb f6                	jmp    106750 <condvar_wait+0x30>
  10675a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  106760:	85 c9                	test   %ecx,%ecx
  106762:	74 14                	je     106778 <condvar_wait+0x58>
  106764:	8b 43 44             	mov    0x44(%ebx),%eax
  106767:	83 f8 0f             	cmp    $0xf,%eax
  10676a:	77 0c                	ja     106778 <condvar_wait+0x58>
  10676c:	8d 50 01             	lea    0x1(%eax),%edx
  10676f:	89 53 44             	mov    %edx,0x44(%ebx)
  106772:	8b 11                	mov    (%ecx),%edx
  106774:	89 54 83 04          	mov    %edx,0x4(%ebx,%eax,4)
  106778:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
  10677e:	57                   	push   %edi
  10677f:	9d                   	popf
  106780:	83 ec 0c             	sub    $0xc,%esp
  106783:	56                   	push   %esi
  106784:	e8 f7 fe ff ff       	call   106680 <mutex_unlock>
  106789:	c7 04 24 05 00 00 00 	movl   $0x5,(%esp)
  106790:	e8 1b fb ff ff       	call   1062b0 <task_sleep>
  106795:	83 c4 10             	add    $0x10,%esp
  106798:	89 74 24 10          	mov    %esi,0x10(%esp)
  10679c:	5b                   	pop    %ebx
  10679d:	5e                   	pop    %esi
  10679e:	5f                   	pop    %edi
  10679f:	e9 cc fd ff ff       	jmp    106570 <mutex_lock>
  1067a4:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1067ab:	00 
  1067ac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

001067b0 <condvar_signal>:
  1067b0:	53                   	push   %ebx
  1067b1:	8b 54 24 08          	mov    0x8(%esp),%edx
  1067b5:	9c                   	pushf
  1067b6:	5b                   	pop    %ebx
  1067b7:	fa                   	cli
  1067b8:	b9 01 00 00 00       	mov    $0x1,%ecx
  1067bd:	8d 76 00             	lea    0x0(%esi),%esi
  1067c0:	89 c8                	mov    %ecx,%eax
  1067c2:	87 02                	xchg   %eax,(%edx)
  1067c4:	85 c0                	test   %eax,%eax
  1067c6:	74 18                	je     1067e0 <condvar_signal+0x30>
  1067c8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1067cf:	00 
  1067d0:	8b 02                	mov    (%edx),%eax
  1067d2:	85 c0                	test   %eax,%eax
  1067d4:	74 ea                	je     1067c0 <condvar_signal+0x10>
  1067d6:	f3 90                	pause
  1067d8:	eb f6                	jmp    1067d0 <condvar_signal+0x20>
  1067da:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1067e0:	8b 42 44             	mov    0x44(%edx),%eax
  1067e3:	85 c0                	test   %eax,%eax
  1067e5:	74 06                	je     1067ed <condvar_signal+0x3d>
  1067e7:	83 e8 01             	sub    $0x1,%eax
  1067ea:	89 42 44             	mov    %eax,0x44(%edx)
  1067ed:	c7 02 00 00 00 00    	movl   $0x0,(%edx)
  1067f3:	53                   	push   %ebx
  1067f4:	9d                   	popf
  1067f5:	5b                   	pop    %ebx
  1067f6:	c3                   	ret
  1067f7:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1067fe:	00 
  1067ff:	90                   	nop

00106800 <condvar_broadcast>:
  106800:	53                   	push   %ebx
  106801:	8b 54 24 08          	mov    0x8(%esp),%edx
  106805:	9c                   	pushf
  106806:	5b                   	pop    %ebx
  106807:	fa                   	cli
  106808:	b9 01 00 00 00       	mov    $0x1,%ecx
  10680d:	8d 76 00             	lea    0x0(%esi),%esi
  106810:	89 c8                	mov    %ecx,%eax
  106812:	87 02                	xchg   %eax,(%edx)
  106814:	85 c0                	test   %eax,%eax
  106816:	74 18                	je     106830 <condvar_broadcast+0x30>
  106818:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10681f:	00 
  106820:	8b 02                	mov    (%edx),%eax
  106822:	85 c0                	test   %eax,%eax
  106824:	74 ea                	je     106810 <condvar_broadcast+0x10>
  106826:	f3 90                	pause
  106828:	eb f6                	jmp    106820 <condvar_broadcast+0x20>
  10682a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  106830:	c7 42 44 00 00 00 00 	movl   $0x0,0x44(%edx)
  106837:	c7 02 00 00 00 00    	movl   $0x0,(%edx)
  10683d:	53                   	push   %ebx
  10683e:	9d                   	popf
  10683f:	5b                   	pop    %ebx
  106840:	c3                   	ret
  106841:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106848:	00 
  106849:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00106850 <rwlock_init>:
  106850:	53                   	push   %ebx
  106851:	83 ec 0c             	sub    $0xc,%esp
  106854:	8b 5c 24 14          	mov    0x14(%esp),%ebx
  106858:	8d 43 10             	lea    0x10(%ebx),%eax
  10685b:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
  106861:	83 c3 5c             	add    $0x5c,%ebx
  106864:	c7 43 a8 00 00 00 00 	movl   $0x0,-0x58(%ebx)
  10686b:	c7 43 f4 00 00 00 00 	movl   $0x0,-0xc(%ebx)
  106872:	c7 43 ac 01 00 00 00 	movl   $0x1,-0x54(%ebx)
  106879:	c7 43 b0 00 00 00 00 	movl   $0x0,-0x50(%ebx)
  106880:	6a 40                	push   $0x40
  106882:	6a 00                	push   $0x0
  106884:	50                   	push   %eax
  106885:	e8 d6 ac ff ff       	call   101560 <memset>
  10688a:	83 c4 0c             	add    $0xc,%esp
  10688d:	c7 43 f8 01 00 00 00 	movl   $0x1,-0x8(%ebx)
  106894:	c7 43 40 00 00 00 00 	movl   $0x0,0x40(%ebx)
  10689b:	c7 43 fc 00 00 00 00 	movl   $0x0,-0x4(%ebx)
  1068a2:	6a 40                	push   $0x40
  1068a4:	6a 00                	push   $0x0
  1068a6:	53                   	push   %ebx
  1068a7:	e8 b4 ac ff ff       	call   101560 <memset>
  1068ac:	83 c4 18             	add    $0x18,%esp
  1068af:	5b                   	pop    %ebx
  1068b0:	c3                   	ret
  1068b1:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1068b8:	00 
  1068b9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

001068c0 <rwlock_read_lock>:
  1068c0:	56                   	push   %esi
  1068c1:	53                   	push   %ebx
  1068c2:	83 ec 04             	sub    $0x4,%esp
  1068c5:	8b 5c 24 10          	mov    0x10(%esp),%ebx
  1068c9:	9c                   	pushf
  1068ca:	5e                   	pop    %esi
  1068cb:	fa                   	cli
  1068cc:	ba 01 00 00 00       	mov    $0x1,%edx
  1068d1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  1068d8:	89 d0                	mov    %edx,%eax
  1068da:	87 03                	xchg   %eax,(%ebx)
  1068dc:	85 c0                	test   %eax,%eax
  1068de:	74 10                	je     1068f0 <rwlock_read_lock+0x30>
  1068e0:	8b 03                	mov    (%ebx),%eax
  1068e2:	85 c0                	test   %eax,%eax
  1068e4:	74 f2                	je     1068d8 <rwlock_read_lock+0x18>
  1068e6:	f3 90                	pause
  1068e8:	eb f6                	jmp    1068e0 <rwlock_read_lock+0x20>
  1068ea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1068f0:	8b 43 04             	mov    0x4(%ebx),%eax
  1068f3:	83 c0 01             	add    $0x1,%eax
  1068f6:	89 43 04             	mov    %eax,0x4(%ebx)
  1068f9:	8b 43 04             	mov    0x4(%ebx),%eax
  1068fc:	83 f8 01             	cmp    $0x1,%eax
  1068ff:	74 0e                	je     10690f <rwlock_read_lock+0x4f>
  106901:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
  106907:	56                   	push   %esi
  106908:	9d                   	popf
  106909:	83 c4 04             	add    $0x4,%esp
  10690c:	5b                   	pop    %ebx
  10690d:	5e                   	pop    %esi
  10690e:	c3                   	ret
  10690f:	83 ec 0c             	sub    $0xc,%esp
  106912:	8d 43 08             	lea    0x8(%ebx),%eax
  106915:	50                   	push   %eax
  106916:	e8 d5 fa ff ff       	call   1063f0 <sem_wait>
  10691b:	83 c4 10             	add    $0x10,%esp
  10691e:	eb e1                	jmp    106901 <rwlock_read_lock+0x41>

00106920 <rwlock_read_unlock>:
  106920:	56                   	push   %esi
  106921:	53                   	push   %ebx
  106922:	8b 5c 24 0c          	mov    0xc(%esp),%ebx
  106926:	9c                   	pushf
  106927:	5e                   	pop    %esi
  106928:	fa                   	cli
  106929:	ba 01 00 00 00       	mov    $0x1,%edx
  10692e:	66 90                	xchg   %ax,%ax
  106930:	89 d0                	mov    %edx,%eax
  106932:	87 03                	xchg   %eax,(%ebx)
  106934:	85 c0                	test   %eax,%eax
  106936:	74 18                	je     106950 <rwlock_read_unlock+0x30>
  106938:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10693f:	00 
  106940:	8b 03                	mov    (%ebx),%eax
  106942:	85 c0                	test   %eax,%eax
  106944:	74 ea                	je     106930 <rwlock_read_unlock+0x10>
  106946:	f3 90                	pause
  106948:	eb f6                	jmp    106940 <rwlock_read_unlock+0x20>
  10694a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  106950:	8b 43 04             	mov    0x4(%ebx),%eax
  106953:	83 e8 01             	sub    $0x1,%eax
  106956:	89 43 04             	mov    %eax,0x4(%ebx)
  106959:	8b 43 04             	mov    0x4(%ebx),%eax
  10695c:	85 c0                	test   %eax,%eax
  10695e:	74 0b                	je     10696b <rwlock_read_unlock+0x4b>
  106960:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
  106966:	56                   	push   %esi
  106967:	9d                   	popf
  106968:	5b                   	pop    %ebx
  106969:	5e                   	pop    %esi
  10696a:	c3                   	ret
  10696b:	8d 43 08             	lea    0x8(%ebx),%eax
  10696e:	50                   	push   %eax
  10696f:	e8 6c fb ff ff       	call   1064e0 <sem_post>
  106974:	58                   	pop    %eax
  106975:	eb e9                	jmp    106960 <rwlock_read_unlock+0x40>
  106977:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10697e:	00 
  10697f:	90                   	nop

00106980 <rwlock_write_lock>:
  106980:	83 44 24 04 08       	addl   $0x8,0x4(%esp)
  106985:	e9 66 fa ff ff       	jmp    1063f0 <sem_wait>
  10698a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00106990 <rwlock_write_unlock>:
  106990:	83 44 24 04 08       	addl   $0x8,0x4(%esp)
  106995:	e9 46 fb ff ff       	jmp    1064e0 <sem_post>
  10699a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

001069a0 <waitq_init>:
  1069a0:	83 ec 10             	sub    $0x10,%esp
  1069a3:	8b 44 24 14          	mov    0x14(%esp),%eax
  1069a7:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
  1069ad:	83 c0 04             	add    $0x4,%eax
  1069b0:	c7 40 40 00 00 00 00 	movl   $0x0,0x40(%eax)
  1069b7:	6a 40                	push   $0x40
  1069b9:	6a 00                	push   $0x0
  1069bb:	50                   	push   %eax
  1069bc:	e8 9f ab ff ff       	call   101560 <memset>
  1069c1:	83 c4 1c             	add    $0x1c,%esp
  1069c4:	c3                   	ret
  1069c5:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1069cc:	00 
  1069cd:	8d 76 00             	lea    0x0(%esi),%esi

001069d0 <waitq_wait>:
  1069d0:	56                   	push   %esi
  1069d1:	53                   	push   %ebx
  1069d2:	83 ec 04             	sub    $0x4,%esp
  1069d5:	8b 5c 24 10          	mov    0x10(%esp),%ebx
  1069d9:	e8 32 f9 ff ff       	call   106310 <sched_current>
  1069de:	89 c1                	mov    %eax,%ecx
  1069e0:	9c                   	pushf
  1069e1:	5e                   	pop    %esi
  1069e2:	fa                   	cli
  1069e3:	ba 01 00 00 00       	mov    $0x1,%edx
  1069e8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1069ef:	00 
  1069f0:	89 d0                	mov    %edx,%eax
  1069f2:	87 03                	xchg   %eax,(%ebx)
  1069f4:	85 c0                	test   %eax,%eax
  1069f6:	74 18                	je     106a10 <waitq_wait+0x40>
  1069f8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1069ff:	00 
  106a00:	8b 03                	mov    (%ebx),%eax
  106a02:	85 c0                	test   %eax,%eax
  106a04:	74 ea                	je     1069f0 <waitq_wait+0x20>
  106a06:	f3 90                	pause
  106a08:	eb f6                	jmp    106a00 <waitq_wait+0x30>
  106a0a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  106a10:	85 c9                	test   %ecx,%ecx
  106a12:	74 14                	je     106a28 <waitq_wait+0x58>
  106a14:	8b 43 44             	mov    0x44(%ebx),%eax
  106a17:	83 f8 0f             	cmp    $0xf,%eax
  106a1a:	77 0c                	ja     106a28 <waitq_wait+0x58>
  106a1c:	8d 50 01             	lea    0x1(%eax),%edx
  106a1f:	89 53 44             	mov    %edx,0x44(%ebx)
  106a22:	8b 11                	mov    (%ecx),%edx
  106a24:	89 54 83 04          	mov    %edx,0x4(%ebx,%eax,4)
  106a28:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
  106a2e:	56                   	push   %esi
  106a2f:	9d                   	popf
  106a30:	c7 44 24 10 0a 00 00 	movl   $0xa,0x10(%esp)
  106a37:	00 
  106a38:	83 c4 04             	add    $0x4,%esp
  106a3b:	5b                   	pop    %ebx
  106a3c:	5e                   	pop    %esi
  106a3d:	e9 6e f8 ff ff       	jmp    1062b0 <task_sleep>
  106a42:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106a49:	00 
  106a4a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00106a50 <waitq_wake_one>:
  106a50:	53                   	push   %ebx
  106a51:	8b 54 24 08          	mov    0x8(%esp),%edx
  106a55:	9c                   	pushf
  106a56:	5b                   	pop    %ebx
  106a57:	fa                   	cli
  106a58:	b9 01 00 00 00       	mov    $0x1,%ecx
  106a5d:	8d 76 00             	lea    0x0(%esi),%esi
  106a60:	89 c8                	mov    %ecx,%eax
  106a62:	87 02                	xchg   %eax,(%edx)
  106a64:	85 c0                	test   %eax,%eax
  106a66:	74 18                	je     106a80 <waitq_wake_one+0x30>
  106a68:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106a6f:	00 
  106a70:	8b 02                	mov    (%edx),%eax
  106a72:	85 c0                	test   %eax,%eax
  106a74:	74 ea                	je     106a60 <waitq_wake_one+0x10>
  106a76:	f3 90                	pause
  106a78:	eb f6                	jmp    106a70 <waitq_wake_one+0x20>
  106a7a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  106a80:	8b 42 44             	mov    0x44(%edx),%eax
  106a83:	85 c0                	test   %eax,%eax
  106a85:	74 06                	je     106a8d <waitq_wake_one+0x3d>
  106a87:	83 e8 01             	sub    $0x1,%eax
  106a8a:	89 42 44             	mov    %eax,0x44(%edx)
  106a8d:	c7 02 00 00 00 00    	movl   $0x0,(%edx)
  106a93:	53                   	push   %ebx
  106a94:	9d                   	popf
  106a95:	5b                   	pop    %ebx
  106a96:	c3                   	ret
  106a97:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106a9e:	00 
  106a9f:	90                   	nop

00106aa0 <waitq_wake_all>:
  106aa0:	53                   	push   %ebx
  106aa1:	8b 54 24 08          	mov    0x8(%esp),%edx
  106aa5:	9c                   	pushf
  106aa6:	5b                   	pop    %ebx
  106aa7:	fa                   	cli
  106aa8:	b9 01 00 00 00       	mov    $0x1,%ecx
  106aad:	8d 76 00             	lea    0x0(%esi),%esi
  106ab0:	89 c8                	mov    %ecx,%eax
  106ab2:	87 02                	xchg   %eax,(%edx)
  106ab4:	85 c0                	test   %eax,%eax
  106ab6:	74 18                	je     106ad0 <waitq_wake_all+0x30>
  106ab8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106abf:	00 
  106ac0:	8b 02                	mov    (%edx),%eax
  106ac2:	85 c0                	test   %eax,%eax
  106ac4:	74 ea                	je     106ab0 <waitq_wake_all+0x10>
  106ac6:	f3 90                	pause
  106ac8:	eb f6                	jmp    106ac0 <waitq_wake_all+0x20>
  106aca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  106ad0:	c7 42 44 00 00 00 00 	movl   $0x0,0x44(%edx)
  106ad7:	c7 02 00 00 00 00    	movl   $0x0,(%edx)
  106add:	53                   	push   %ebx
  106ade:	9d                   	popf
  106adf:	5b                   	pop    %ebx
  106ae0:	c3                   	ret
  106ae1:	66 90                	xchg   %ax,%ax
  106ae3:	66 90                	xchg   %ax,%ax
  106ae5:	66 90                	xchg   %ax,%ax
  106ae7:	66 90                	xchg   %ax,%ax
  106ae9:	66 90                	xchg   %ax,%ax
  106aeb:	66 90                	xchg   %ax,%ax
  106aed:	66 90                	xchg   %ax,%ax
  106aef:	90                   	nop

00106af0 <vfs_init>:
  106af0:	83 ec 10             	sub    $0x10,%esp
  106af3:	68 c0 10 00 00       	push   $0x10c0
  106af8:	6a 00                	push   $0x0
  106afa:	68 80 9f 11 00       	push   $0x119f80
  106aff:	e8 5c aa ff ff       	call   101560 <memset>
  106b04:	83 c4 0c             	add    $0xc,%esp
  106b07:	6a 40                	push   $0x40
  106b09:	6a 00                	push   $0x0
  106b0b:	68 40 9f 11 00       	push   $0x119f40
  106b10:	e8 4b aa ff ff       	call   101560 <memset>
  106b15:	58                   	pop    %eax
  106b16:	5a                   	pop    %edx
  106b17:	68 f7 94 10 00       	push   $0x1094f7
  106b1c:	6a 01                	push   $0x1
  106b1e:	c7 05 20 9f 11 00 00 	movl   $0x0,0x119f20
  106b25:	00 00 00 
  106b28:	e8 03 e0 ff ff       	call   104b30 <klog>
  106b2d:	83 c4 1c             	add    $0x1c,%esp
  106b30:	c3                   	ret
  106b31:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106b38:	00 
  106b39:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00106b40 <vfs_register_fs>:
  106b40:	a1 20 9f 11 00       	mov    0x119f20,%eax
  106b45:	83 f8 0f             	cmp    $0xf,%eax
  106b48:	76 06                	jbe    106b50 <vfs_register_fs+0x10>
  106b4a:	c3                   	ret
  106b4b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  106b50:	83 ec 10             	sub    $0x10,%esp
  106b53:	8d 50 01             	lea    0x1(%eax),%edx
  106b56:	8b 4c 24 14          	mov    0x14(%esp),%ecx
  106b5a:	89 15 20 9f 11 00    	mov    %edx,0x119f20
  106b60:	ff 31                	push   (%ecx)
  106b62:	68 60 9e 10 00       	push   $0x109e60
  106b67:	6a 01                	push   $0x1
  106b69:	89 0c 85 40 9f 11 00 	mov    %ecx,0x119f40(,%eax,4)
  106b70:	e8 bb df ff ff       	call   104b30 <klog>
  106b75:	83 c4 1c             	add    $0x1c,%esp
  106b78:	c3                   	ret
  106b79:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00106b80 <vfs_mount>:
  106b80:	55                   	push   %ebp
  106b81:	b8 88 a0 11 00       	mov    $0x11a088,%eax
  106b86:	57                   	push   %edi
  106b87:	31 ff                	xor    %edi,%edi
  106b89:	56                   	push   %esi
  106b8a:	53                   	push   %ebx
  106b8b:	83 ec 1c             	sub    $0x1c,%esp
  106b8e:	8b 74 24 38          	mov    0x38(%esp),%esi
  106b92:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  106b98:	80 38 00             	cmpb   $0x0,(%eax)
  106b9b:	74 23                	je     106bc0 <vfs_mount+0x40>
  106b9d:	83 c7 01             	add    $0x1,%edi
  106ba0:	05 0c 01 00 00       	add    $0x10c,%eax
  106ba5:	83 ff 10             	cmp    $0x10,%edi
  106ba8:	75 ee                	jne    106b98 <vfs_mount+0x18>
  106baa:	83 c4 1c             	add    $0x1c,%esp
  106bad:	bb ff ff ff ff       	mov    $0xffffffff,%ebx
  106bb2:	89 d8                	mov    %ebx,%eax
  106bb4:	5b                   	pop    %ebx
  106bb5:	5e                   	pop    %esi
  106bb6:	5f                   	pop    %edi
  106bb7:	5d                   	pop    %ebp
  106bb8:	c3                   	ret
  106bb9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  106bc0:	69 c7 0c 01 00 00    	imul   $0x10c,%edi,%eax
  106bc6:	8b 15 20 9f 11 00    	mov    0x119f20,%edx
  106bcc:	31 ed                	xor    %ebp,%ebp
  106bce:	89 44 24 08          	mov    %eax,0x8(%esp)
  106bd2:	85 d2                	test   %edx,%edx
  106bd4:	75 19                	jne    106bef <vfs_mount+0x6f>
  106bd6:	e9 b5 00 00 00       	jmp    106c90 <vfs_mount+0x110>
  106bdb:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  106be0:	83 c5 01             	add    $0x1,%ebp
  106be3:	3b 2d 20 9f 11 00    	cmp    0x119f20,%ebp
  106be9:	0f 83 a1 00 00 00    	jae    106c90 <vfs_mount+0x110>
  106bef:	8b 04 ad 40 9f 11 00 	mov    0x119f40(,%ebp,4),%eax
  106bf6:	83 ec 08             	sub    $0x8,%esp
  106bf9:	56                   	push   %esi
  106bfa:	ff 30                	push   (%eax)
  106bfc:	e8 6f ab ff ff       	call   101770 <strcmp>
  106c01:	83 c4 10             	add    $0x10,%esp
  106c04:	85 c0                	test   %eax,%eax
  106c06:	75 d8                	jne    106be0 <vfs_mount+0x60>
  106c08:	8b 2c ad 40 9f 11 00 	mov    0x119f40(,%ebp,4),%ebp
  106c0f:	89 c3                	mov    %eax,%ebx
  106c11:	85 ed                	test   %ebp,%ebp
  106c13:	74 7b                	je     106c90 <vfs_mount+0x110>
  106c15:	8b 45 24             	mov    0x24(%ebp),%eax
  106c18:	85 c0                	test   %eax,%eax
  106c1a:	74 74                	je     106c90 <vfs_mount+0x110>
  106c1c:	83 ec 08             	sub    $0x8,%esp
  106c1f:	ff 74 24 44          	push   0x44(%esp)
  106c23:	ff 74 24 40          	push   0x40(%esp)
  106c27:	ff d0                	call   *%eax
  106c29:	83 c4 10             	add    $0x10,%esp
  106c2c:	85 c0                	test   %eax,%eax
  106c2e:	89 44 24 0c          	mov    %eax,0xc(%esp)
  106c32:	74 63                	je     106c97 <vfs_mount+0x117>
  106c34:	83 ec 04             	sub    $0x4,%esp
  106c37:	68 ff 00 00 00       	push   $0xff
  106c3c:	ff 74 24 38          	push   0x38(%esp)
  106c40:	8b 44 24 14          	mov    0x14(%esp),%eax
  106c44:	05 80 9f 11 00       	add    $0x119f80,%eax
  106c49:	50                   	push   %eax
  106c4a:	e8 a1 aa ff ff       	call   1016f0 <strncpy>
  106c4f:	8b 4c 24 1c          	mov    0x1c(%esp),%ecx
  106c53:	69 d7 0c 01 00 00    	imul   $0x10c,%edi,%edx
  106c59:	58                   	pop    %eax
  106c5a:	ff 74 24 3c          	push   0x3c(%esp)
  106c5e:	56                   	push   %esi
  106c5f:	ff 74 24 48          	push   0x48(%esp)
  106c63:	68 84 9e 10 00       	push   $0x109e84
  106c68:	6a 01                	push   $0x1
  106c6a:	89 8a 80 a0 11 00    	mov    %ecx,0x11a080(%edx)
  106c70:	89 aa 84 a0 11 00    	mov    %ebp,0x11a084(%edx)
  106c76:	c6 82 88 a0 11 00 01 	movb   $0x1,0x11a088(%edx)
  106c7d:	e8 ae de ff ff       	call   104b30 <klog>
  106c82:	83 c4 20             	add    $0x20,%esp
  106c85:	83 c4 1c             	add    $0x1c,%esp
  106c88:	89 d8                	mov    %ebx,%eax
  106c8a:	5b                   	pop    %ebx
  106c8b:	5e                   	pop    %esi
  106c8c:	5f                   	pop    %edi
  106c8d:	5d                   	pop    %ebp
  106c8e:	c3                   	ret
  106c8f:	90                   	nop
  106c90:	bb fe ff ff ff       	mov    $0xfffffffe,%ebx
  106c95:	eb ee                	jmp    106c85 <vfs_mount+0x105>
  106c97:	bb fd ff ff ff       	mov    $0xfffffffd,%ebx
  106c9c:	eb e7                	jmp    106c85 <vfs_mount+0x105>
  106c9e:	66 90                	xchg   %ax,%ax

00106ca0 <vfs_lookup>:
  106ca0:	55                   	push   %ebp
  106ca1:	31 ed                	xor    %ebp,%ebp
  106ca3:	57                   	push   %edi
  106ca4:	31 ff                	xor    %edi,%edi
  106ca6:	56                   	push   %esi
  106ca7:	53                   	push   %ebx
  106ca8:	bb 80 9f 11 00       	mov    $0x119f80,%ebx
  106cad:	81 ec 0c 01 00 00    	sub    $0x10c,%esp
  106cb3:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  106cb8:	80 bb 08 01 00 00 00 	cmpb   $0x0,0x108(%ebx)
  106cbf:	74 30                	je     106cf1 <vfs_lookup+0x51>
  106cc1:	83 ec 0c             	sub    $0xc,%esp
  106cc4:	53                   	push   %ebx
  106cc5:	e8 c6 a9 ff ff       	call   101690 <strlen>
  106cca:	83 c4 0c             	add    $0xc,%esp
  106ccd:	50                   	push   %eax
  106cce:	89 c6                	mov    %eax,%esi
  106cd0:	ff b4 24 28 01 00 00 	push   0x128(%esp)
  106cd7:	53                   	push   %ebx
  106cd8:	e8 e3 aa ff ff       	call   1017c0 <strncmp>
  106cdd:	83 c4 10             	add    $0x10,%esp
  106ce0:	39 f5                	cmp    %esi,%ebp
  106ce2:	73 0d                	jae    106cf1 <vfs_lookup+0x51>
  106ce4:	85 c0                	test   %eax,%eax
  106ce6:	0f 94 c0             	sete   %al
  106ce9:	84 c0                	test   %al,%al
  106ceb:	0f 45 ee             	cmovne %esi,%ebp
  106cee:	0f 45 fb             	cmovne %ebx,%edi
  106cf1:	81 c3 0c 01 00 00    	add    $0x10c,%ebx
  106cf7:	81 fb 40 b0 11 00    	cmp    $0x11b040,%ebx
  106cfd:	75 b9                	jne    106cb8 <vfs_lookup+0x18>
  106cff:	85 ff                	test   %edi,%edi
  106d01:	0f 84 9c 00 00 00    	je     106da3 <vfs_lookup+0x103>
  106d07:	8b 9c 24 20 01 00 00 	mov    0x120(%esp),%ebx
  106d0e:	8b b7 00 01 00 00    	mov    0x100(%edi),%esi
  106d14:	01 eb                	add    %ebp,%ebx
  106d16:	0f b6 03             	movzbl (%ebx),%eax
  106d19:	3c 2f                	cmp    $0x2f,%al
  106d1b:	75 0e                	jne    106d2b <vfs_lookup+0x8b>
  106d1d:	8d 76 00             	lea    0x0(%esi),%esi
  106d20:	0f b6 43 01          	movzbl 0x1(%ebx),%eax
  106d24:	83 c3 01             	add    $0x1,%ebx
  106d27:	3c 2f                	cmp    $0x2f,%al
  106d29:	74 f5                	je     106d20 <vfs_lookup+0x80>
  106d2b:	84 c0                	test   %al,%al
  106d2d:	74 76                	je     106da5 <vfs_lookup+0x105>
  106d2f:	89 e7                	mov    %esp,%edi
  106d31:	85 f6                	test   %esi,%esi
  106d33:	74 6e                	je     106da3 <vfs_lookup+0x103>
  106d35:	8d 76 00             	lea    0x0(%esi),%esi
  106d38:	83 ec 08             	sub    $0x8,%esp
  106d3b:	6a 2f                	push   $0x2f
  106d3d:	53                   	push   %ebx
  106d3e:	e8 cd aa ff ff       	call   101810 <strchr>
  106d43:	83 c4 10             	add    $0x10,%esp
  106d46:	89 c5                	mov    %eax,%ebp
  106d48:	85 c0                	test   %eax,%eax
  106d4a:	74 6c                	je     106db8 <vfs_lookup+0x118>
  106d4c:	29 dd                	sub    %ebx,%ebp
  106d4e:	81 fd ff 00 00 00    	cmp    $0xff,%ebp
  106d54:	77 4d                	ja     106da3 <vfs_lookup+0x103>
  106d56:	83 ec 04             	sub    $0x4,%esp
  106d59:	55                   	push   %ebp
  106d5a:	53                   	push   %ebx
  106d5b:	57                   	push   %edi
  106d5c:	e8 8f a9 ff ff       	call   1016f0 <strncpy>
  106d61:	8b 86 24 01 00 00    	mov    0x124(%esi),%eax
  106d67:	c6 44 2c 10 00       	movb   $0x0,0x10(%esp,%ebp,1)
  106d6c:	83 c4 10             	add    $0x10,%esp
  106d6f:	85 c0                	test   %eax,%eax
  106d71:	74 30                	je     106da3 <vfs_lookup+0x103>
  106d73:	8b 40 18             	mov    0x18(%eax),%eax
  106d76:	85 c0                	test   %eax,%eax
  106d78:	74 29                	je     106da3 <vfs_lookup+0x103>
  106d7a:	83 ec 08             	sub    $0x8,%esp
  106d7d:	01 eb                	add    %ebp,%ebx
  106d7f:	57                   	push   %edi
  106d80:	56                   	push   %esi
  106d81:	ff d0                	call   *%eax
  106d83:	89 c6                	mov    %eax,%esi
  106d85:	0f b6 03             	movzbl (%ebx),%eax
  106d88:	83 c4 10             	add    $0x10,%esp
  106d8b:	3c 2f                	cmp    $0x2f,%al
  106d8d:	75 0c                	jne    106d9b <vfs_lookup+0xfb>
  106d8f:	90                   	nop
  106d90:	0f b6 43 01          	movzbl 0x1(%ebx),%eax
  106d94:	83 c3 01             	add    $0x1,%ebx
  106d97:	3c 2f                	cmp    $0x2f,%al
  106d99:	74 f5                	je     106d90 <vfs_lookup+0xf0>
  106d9b:	84 c0                	test   %al,%al
  106d9d:	74 06                	je     106da5 <vfs_lookup+0x105>
  106d9f:	85 f6                	test   %esi,%esi
  106da1:	75 95                	jne    106d38 <vfs_lookup+0x98>
  106da3:	31 f6                	xor    %esi,%esi
  106da5:	81 c4 0c 01 00 00    	add    $0x10c,%esp
  106dab:	89 f0                	mov    %esi,%eax
  106dad:	5b                   	pop    %ebx
  106dae:	5e                   	pop    %esi
  106daf:	5f                   	pop    %edi
  106db0:	5d                   	pop    %ebp
  106db1:	c3                   	ret
  106db2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  106db8:	83 ec 0c             	sub    $0xc,%esp
  106dbb:	53                   	push   %ebx
  106dbc:	e8 cf a8 ff ff       	call   101690 <strlen>
  106dc1:	83 c4 10             	add    $0x10,%esp
  106dc4:	89 c5                	mov    %eax,%ebp
  106dc6:	eb 86                	jmp    106d4e <vfs_lookup+0xae>
  106dc8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106dcf:	00 

00106dd0 <vfs_open>:
  106dd0:	83 ec 18             	sub    $0x18,%esp
  106dd3:	ff 74 24 1c          	push   0x1c(%esp)
  106dd7:	e8 c4 fe ff ff       	call   106ca0 <vfs_lookup>
  106ddc:	83 c4 10             	add    $0x10,%esp
  106ddf:	85 c0                	test   %eax,%eax
  106de1:	74 07                	je     106dea <vfs_open+0x1a>
  106de3:	83 80 30 01 00 00 01 	addl   $0x1,0x130(%eax)
  106dea:	83 c4 0c             	add    $0xc,%esp
  106ded:	c3                   	ret
  106dee:	66 90                	xchg   %ax,%ax

00106df0 <vfs_close>:
  106df0:	8b 44 24 04          	mov    0x4(%esp),%eax
  106df4:	85 c0                	test   %eax,%eax
  106df6:	74 28                	je     106e20 <vfs_close+0x30>
  106df8:	8b 90 30 01 00 00    	mov    0x130(%eax),%edx
  106dfe:	85 d2                	test   %edx,%edx
  106e00:	75 26                	jne    106e28 <vfs_close+0x38>
  106e02:	8b 90 24 01 00 00    	mov    0x124(%eax),%edx
  106e08:	85 d2                	test   %edx,%edx
  106e0a:	74 14                	je     106e20 <vfs_close+0x30>
  106e0c:	8b 52 10             	mov    0x10(%edx),%edx
  106e0f:	85 d2                	test   %edx,%edx
  106e11:	74 0d                	je     106e20 <vfs_close+0x30>
  106e13:	89 44 24 04          	mov    %eax,0x4(%esp)
  106e17:	ff e2                	jmp    *%edx
  106e19:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  106e20:	c3                   	ret
  106e21:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  106e28:	83 ea 01             	sub    $0x1,%edx
  106e2b:	89 90 30 01 00 00    	mov    %edx,0x130(%eax)
  106e31:	eb cf                	jmp    106e02 <vfs_close+0x12>
  106e33:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106e3a:	00 
  106e3b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00106e40 <vfs_read>:
  106e40:	56                   	push   %esi
  106e41:	53                   	push   %ebx
  106e42:	8b 44 24 0c          	mov    0xc(%esp),%eax
  106e46:	8b 4c 24 10          	mov    0x10(%esp),%ecx
  106e4a:	8b 5c 24 14          	mov    0x14(%esp),%ebx
  106e4e:	8b 74 24 18          	mov    0x18(%esp),%esi
  106e52:	85 c0                	test   %eax,%eax
  106e54:	74 2a                	je     106e80 <vfs_read+0x40>
  106e56:	8b 90 24 01 00 00    	mov    0x124(%eax),%edx
  106e5c:	85 d2                	test   %edx,%edx
  106e5e:	74 20                	je     106e80 <vfs_read+0x40>
  106e60:	8b 52 04             	mov    0x4(%edx),%edx
  106e63:	85 d2                	test   %edx,%edx
  106e65:	74 19                	je     106e80 <vfs_read+0x40>
  106e67:	89 74 24 18          	mov    %esi,0x18(%esp)
  106e6b:	89 5c 24 14          	mov    %ebx,0x14(%esp)
  106e6f:	89 4c 24 10          	mov    %ecx,0x10(%esp)
  106e73:	89 44 24 0c          	mov    %eax,0xc(%esp)
  106e77:	5b                   	pop    %ebx
  106e78:	5e                   	pop    %esi
  106e79:	ff e2                	jmp    *%edx
  106e7b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  106e80:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  106e85:	5b                   	pop    %ebx
  106e86:	5e                   	pop    %esi
  106e87:	c3                   	ret
  106e88:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106e8f:	00 

00106e90 <vfs_write>:
  106e90:	56                   	push   %esi
  106e91:	53                   	push   %ebx
  106e92:	8b 44 24 0c          	mov    0xc(%esp),%eax
  106e96:	8b 4c 24 10          	mov    0x10(%esp),%ecx
  106e9a:	8b 5c 24 14          	mov    0x14(%esp),%ebx
  106e9e:	8b 74 24 18          	mov    0x18(%esp),%esi
  106ea2:	85 c0                	test   %eax,%eax
  106ea4:	74 2a                	je     106ed0 <vfs_write+0x40>
  106ea6:	8b 90 24 01 00 00    	mov    0x124(%eax),%edx
  106eac:	85 d2                	test   %edx,%edx
  106eae:	74 20                	je     106ed0 <vfs_write+0x40>
  106eb0:	8b 52 08             	mov    0x8(%edx),%edx
  106eb3:	85 d2                	test   %edx,%edx
  106eb5:	74 19                	je     106ed0 <vfs_write+0x40>
  106eb7:	89 74 24 18          	mov    %esi,0x18(%esp)
  106ebb:	89 5c 24 14          	mov    %ebx,0x14(%esp)
  106ebf:	89 4c 24 10          	mov    %ecx,0x10(%esp)
  106ec3:	89 44 24 0c          	mov    %eax,0xc(%esp)
  106ec7:	5b                   	pop    %ebx
  106ec8:	5e                   	pop    %esi
  106ec9:	ff e2                	jmp    *%edx
  106ecb:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  106ed0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  106ed5:	5b                   	pop    %ebx
  106ed6:	5e                   	pop    %esi
  106ed7:	c3                   	ret
  106ed8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106edf:	00 

00106ee0 <vfs_readdir>:
  106ee0:	53                   	push   %ebx
  106ee1:	8b 44 24 08          	mov    0x8(%esp),%eax
  106ee5:	8b 4c 24 0c          	mov    0xc(%esp),%ecx
  106ee9:	8b 5c 24 10          	mov    0x10(%esp),%ebx
  106eed:	85 c0                	test   %eax,%eax
  106eef:	74 27                	je     106f18 <vfs_readdir+0x38>
  106ef1:	8b 90 24 01 00 00    	mov    0x124(%eax),%edx
  106ef7:	85 d2                	test   %edx,%edx
  106ef9:	74 1d                	je     106f18 <vfs_readdir+0x38>
  106efb:	8b 52 14             	mov    0x14(%edx),%edx
  106efe:	85 d2                	test   %edx,%edx
  106f00:	74 16                	je     106f18 <vfs_readdir+0x38>
  106f02:	89 5c 24 10          	mov    %ebx,0x10(%esp)
  106f06:	89 4c 24 0c          	mov    %ecx,0xc(%esp)
  106f0a:	89 44 24 08          	mov    %eax,0x8(%esp)
  106f0e:	5b                   	pop    %ebx
  106f0f:	ff e2                	jmp    *%edx
  106f11:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  106f18:	31 c0                	xor    %eax,%eax
  106f1a:	5b                   	pop    %ebx
  106f1b:	c3                   	ret
  106f1c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00106f20 <vfs_finddir>:
  106f20:	8b 44 24 04          	mov    0x4(%esp),%eax
  106f24:	8b 4c 24 08          	mov    0x8(%esp),%ecx
  106f28:	85 c0                	test   %eax,%eax
  106f2a:	74 24                	je     106f50 <vfs_finddir+0x30>
  106f2c:	8b 90 24 01 00 00    	mov    0x124(%eax),%edx
  106f32:	85 d2                	test   %edx,%edx
  106f34:	74 1a                	je     106f50 <vfs_finddir+0x30>
  106f36:	8b 52 18             	mov    0x18(%edx),%edx
  106f39:	85 d2                	test   %edx,%edx
  106f3b:	74 13                	je     106f50 <vfs_finddir+0x30>
  106f3d:	89 4c 24 08          	mov    %ecx,0x8(%esp)
  106f41:	89 44 24 04          	mov    %eax,0x4(%esp)
  106f45:	ff e2                	jmp    *%edx
  106f47:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106f4e:	00 
  106f4f:	90                   	nop
  106f50:	31 c0                	xor    %eax,%eax
  106f52:	c3                   	ret
  106f53:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106f5a:	00 
  106f5b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00106f60 <vfs_umount>:
  106f60:	57                   	push   %edi
  106f61:	56                   	push   %esi
  106f62:	31 f6                	xor    %esi,%esi
  106f64:	53                   	push   %ebx
  106f65:	bb 80 9f 11 00       	mov    $0x119f80,%ebx
  106f6a:	8b 7c 24 10          	mov    0x10(%esp),%edi
  106f6e:	eb 0e                	jmp    106f7e <vfs_umount+0x1e>
  106f70:	83 c6 01             	add    $0x1,%esi
  106f73:	81 c3 0c 01 00 00    	add    $0x10c,%ebx
  106f79:	83 fe 10             	cmp    $0x10,%esi
  106f7c:	74 32                	je     106fb0 <vfs_umount+0x50>
  106f7e:	80 bb 08 01 00 00 00 	cmpb   $0x0,0x108(%ebx)
  106f85:	74 e9                	je     106f70 <vfs_umount+0x10>
  106f87:	83 ec 08             	sub    $0x8,%esp
  106f8a:	57                   	push   %edi
  106f8b:	53                   	push   %ebx
  106f8c:	e8 df a7 ff ff       	call   101770 <strcmp>
  106f91:	83 c4 10             	add    $0x10,%esp
  106f94:	85 c0                	test   %eax,%eax
  106f96:	75 d8                	jne    106f70 <vfs_umount+0x10>
  106f98:	69 f6 0c 01 00 00    	imul   $0x10c,%esi,%esi
  106f9e:	5b                   	pop    %ebx
  106f9f:	c6 86 88 a0 11 00 00 	movb   $0x0,0x11a088(%esi)
  106fa6:	5e                   	pop    %esi
  106fa7:	5f                   	pop    %edi
  106fa8:	c3                   	ret
  106fa9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  106fb0:	5b                   	pop    %ebx
  106fb1:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  106fb6:	5e                   	pop    %esi
  106fb7:	5f                   	pop    %edi
  106fb8:	c3                   	ret
  106fb9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00106fc0 <vfs_dump>:
  106fc0:	53                   	push   %ebx
  106fc1:	bb 80 9f 11 00       	mov    $0x119f80,%ebx
  106fc6:	83 ec 10             	sub    $0x10,%esp
  106fc9:	68 0b 95 10 00       	push   $0x10950b
  106fce:	6a 01                	push   $0x1
  106fd0:	e8 5b db ff ff       	call   104b30 <klog>
  106fd5:	83 c4 10             	add    $0x10,%esp
  106fd8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  106fdf:	00 
  106fe0:	80 bb 08 01 00 00 00 	cmpb   $0x0,0x108(%ebx)
  106fe7:	74 22                	je     10700b <vfs_dump+0x4b>
  106fe9:	8b 93 04 01 00 00    	mov    0x104(%ebx),%edx
  106fef:	b8 86 93 10 00       	mov    $0x109386,%eax
  106ff4:	85 d2                	test   %edx,%edx
  106ff6:	74 02                	je     106ffa <vfs_dump+0x3a>
  106ff8:	8b 02                	mov    (%edx),%eax
  106ffa:	50                   	push   %eax
  106ffb:	53                   	push   %ebx
  106ffc:	68 1f 95 10 00       	push   $0x10951f
  107001:	6a 01                	push   $0x1
  107003:	e8 28 db ff ff       	call   104b30 <klog>
  107008:	83 c4 10             	add    $0x10,%esp
  10700b:	81 c3 0c 01 00 00    	add    $0x10c,%ebx
  107011:	81 fb 40 b0 11 00    	cmp    $0x11b040,%ebx
  107017:	75 c7                	jne    106fe0 <vfs_dump+0x20>
  107019:	83 c4 08             	add    $0x8,%esp
  10701c:	5b                   	pop    %ebx
  10701d:	c3                   	ret
  10701e:	66 90                	xchg   %ax,%ax

00107020 <ramfs_finddir>:
  107020:	57                   	push   %edi
  107021:	56                   	push   %esi
  107022:	53                   	push   %ebx
  107023:	8b 44 24 10          	mov    0x10(%esp),%eax
  107027:	8b 7c 24 14          	mov    0x14(%esp),%edi
  10702b:	8b 80 28 01 00 00    	mov    0x128(%eax),%eax
  107031:	8b 98 3c 01 00 00    	mov    0x13c(%eax),%ebx
  107037:	85 db                	test   %ebx,%ebx
  107039:	75 0f                	jne    10704a <ramfs_finddir+0x2a>
  10703b:	eb 2b                	jmp    107068 <ramfs_finddir+0x48>
  10703d:	8d 76 00             	lea    0x0(%esi),%esi
  107040:	8b 9b 40 01 00 00    	mov    0x140(%ebx),%ebx
  107046:	85 db                	test   %ebx,%ebx
  107048:	74 1e                	je     107068 <ramfs_finddir+0x48>
  10704a:	83 ec 08             	sub    $0x8,%esp
  10704d:	89 de                	mov    %ebx,%esi
  10704f:	57                   	push   %edi
  107050:	53                   	push   %ebx
  107051:	e8 1a a7 ff ff       	call   101770 <strcmp>
  107056:	83 c4 10             	add    $0x10,%esp
  107059:	85 c0                	test   %eax,%eax
  10705b:	75 e3                	jne    107040 <ramfs_finddir+0x20>
  10705d:	89 f0                	mov    %esi,%eax
  10705f:	5b                   	pop    %ebx
  107060:	5e                   	pop    %esi
  107061:	5f                   	pop    %edi
  107062:	c3                   	ret
  107063:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  107068:	31 f6                	xor    %esi,%esi
  10706a:	5b                   	pop    %ebx
  10706b:	89 f0                	mov    %esi,%eax
  10706d:	5e                   	pop    %esi
  10706e:	5f                   	pop    %edi
  10706f:	c3                   	ret

00107070 <ramfs_unlink>:
  107070:	56                   	push   %esi
  107071:	53                   	push   %ebx
  107072:	83 ec 14             	sub    $0x14,%esp
  107075:	8b 44 24 20          	mov    0x20(%esp),%eax
  107079:	8b 5c 24 24          	mov    0x24(%esp),%ebx
  10707d:	8b 80 28 01 00 00    	mov    0x128(%eax),%eax
  107083:	8d b0 3c 01 00 00    	lea    0x13c(%eax),%esi
  107089:	8b 80 3c 01 00 00    	mov    0x13c(%eax),%eax
  10708f:	85 c0                	test   %eax,%eax
  107091:	75 17                	jne    1070aa <ramfs_unlink+0x3a>
  107093:	eb 5b                	jmp    1070f0 <ramfs_unlink+0x80>
  107095:	8d 76 00             	lea    0x0(%esi),%esi
  107098:	8b 06                	mov    (%esi),%eax
  10709a:	8d b0 40 01 00 00    	lea    0x140(%eax),%esi
  1070a0:	8b 80 40 01 00 00    	mov    0x140(%eax),%eax
  1070a6:	85 c0                	test   %eax,%eax
  1070a8:	74 46                	je     1070f0 <ramfs_unlink+0x80>
  1070aa:	83 ec 08             	sub    $0x8,%esp
  1070ad:	53                   	push   %ebx
  1070ae:	50                   	push   %eax
  1070af:	e8 bc a6 ff ff       	call   101770 <strcmp>
  1070b4:	83 c4 10             	add    $0x10,%esp
  1070b7:	85 c0                	test   %eax,%eax
  1070b9:	75 dd                	jne    107098 <ramfs_unlink+0x28>
  1070bb:	8b 1e                	mov    (%esi),%ebx
  1070bd:	89 44 24 0c          	mov    %eax,0xc(%esp)
  1070c1:	83 ec 0c             	sub    $0xc,%esp
  1070c4:	8b 93 40 01 00 00    	mov    0x140(%ebx),%edx
  1070ca:	89 16                	mov    %edx,(%esi)
  1070cc:	ff b3 34 01 00 00    	push   0x134(%ebx)
  1070d2:	e8 89 c4 ff ff       	call   103560 <kfree>
  1070d7:	89 1c 24             	mov    %ebx,(%esp)
  1070da:	e8 81 c4 ff ff       	call   103560 <kfree>
  1070df:	83 c4 10             	add    $0x10,%esp
  1070e2:	8b 44 24 0c          	mov    0xc(%esp),%eax
  1070e6:	83 c4 14             	add    $0x14,%esp
  1070e9:	5b                   	pop    %ebx
  1070ea:	5e                   	pop    %esi
  1070eb:	c3                   	ret
  1070ec:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  1070f0:	83 c4 14             	add    $0x14,%esp
  1070f3:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  1070f8:	5b                   	pop    %ebx
  1070f9:	5e                   	pop    %esi
  1070fa:	c3                   	ret
  1070fb:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

00107100 <ramfs_readdir>:
  107100:	55                   	push   %ebp
  107101:	57                   	push   %edi
  107102:	56                   	push   %esi
  107103:	53                   	push   %ebx
  107104:	83 ec 1c             	sub    $0x1c,%esp
  107107:	8b 6c 24 30          	mov    0x30(%esp),%ebp
  10710b:	8b 7c 24 34          	mov    0x34(%esp),%edi
  10710f:	8b 74 24 38          	mov    0x38(%esp),%esi
  107113:	8b 95 28 01 00 00    	mov    0x128(%ebp),%edx
  107119:	8b 9a 3c 01 00 00    	mov    0x13c(%edx),%ebx
  10711f:	85 ff                	test   %edi,%edi
  107121:	74 75                	je     107198 <ramfs_readdir+0x98>
  107123:	83 ff 01             	cmp    $0x1,%edi
  107126:	0f 84 94 00 00 00    	je     1071c0 <ramfs_readdir+0xc0>
  10712c:	85 db                	test   %ebx,%ebx
  10712e:	74 23                	je     107153 <ramfs_readdir+0x53>
  107130:	8d 47 fd             	lea    -0x3(%edi),%eax
  107133:	83 ff 02             	cmp    $0x2,%edi
  107136:	75 11                	jne    107149 <ramfs_readdir+0x49>
  107138:	eb 26                	jmp    107160 <ramfs_readdir+0x60>
  10713a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  107140:	8d 50 ff             	lea    -0x1(%eax),%edx
  107143:	85 c0                	test   %eax,%eax
  107145:	74 19                	je     107160 <ramfs_readdir+0x60>
  107147:	89 d0                	mov    %edx,%eax
  107149:	8b 9b 40 01 00 00    	mov    0x140(%ebx),%ebx
  10714f:	85 db                	test   %ebx,%ebx
  107151:	75 ed                	jne    107140 <ramfs_readdir+0x40>
  107153:	31 c0                	xor    %eax,%eax
  107155:	83 c4 1c             	add    $0x1c,%esp
  107158:	5b                   	pop    %ebx
  107159:	5e                   	pop    %esi
  10715a:	5f                   	pop    %edi
  10715b:	5d                   	pop    %ebp
  10715c:	c3                   	ret
  10715d:	8d 76 00             	lea    0x0(%esi),%esi
  107160:	83 ec 04             	sub    $0x4,%esp
  107163:	68 ff 00 00 00       	push   $0xff
  107168:	53                   	push   %ebx
  107169:	56                   	push   %esi
  10716a:	e8 81 a5 ff ff       	call   1016f0 <strncpy>
  10716f:	8b 83 00 01 00 00    	mov    0x100(%ebx),%eax
  107175:	83 c4 10             	add    $0x10,%esp
  107178:	89 86 00 01 00 00    	mov    %eax,0x100(%esi)
  10717e:	0f b6 83 04 01 00 00 	movzbl 0x104(%ebx),%eax
  107185:	88 86 04 01 00 00    	mov    %al,0x104(%esi)
  10718b:	b8 01 00 00 00       	mov    $0x1,%eax
  107190:	eb c3                	jmp    107155 <ramfs_readdir+0x55>
  107192:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  107198:	83 ec 08             	sub    $0x8,%esp
  10719b:	68 6f 90 10 00       	push   $0x10906f
  1071a0:	56                   	push   %esi
  1071a1:	e8 1a a5 ff ff       	call   1016c0 <strcpy>
  1071a6:	8b 85 00 01 00 00    	mov    0x100(%ebp),%eax
  1071ac:	83 c4 10             	add    $0x10,%esp
  1071af:	89 86 00 01 00 00    	mov    %eax,0x100(%esi)
  1071b5:	b8 02 00 00 00       	mov    $0x2,%eax
  1071ba:	eb c9                	jmp    107185 <ramfs_readdir+0x85>
  1071bc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  1071c0:	89 54 24 0c          	mov    %edx,0xc(%esp)
  1071c4:	83 ec 08             	sub    $0x8,%esp
  1071c7:	68 6e 90 10 00       	push   $0x10906e
  1071cc:	56                   	push   %esi
  1071cd:	e8 ee a4 ff ff       	call   1016c0 <strcpy>
  1071d2:	8b 54 24 1c          	mov    0x1c(%esp),%edx
  1071d6:	83 c4 10             	add    $0x10,%esp
  1071d9:	8b 82 44 01 00 00    	mov    0x144(%edx),%eax
  1071df:	85 c0                	test   %eax,%eax
  1071e1:	74 06                	je     1071e9 <ramfs_readdir+0xe9>
  1071e3:	8b b8 00 01 00 00    	mov    0x100(%eax),%edi
  1071e9:	89 be 00 01 00 00    	mov    %edi,0x100(%esi)
  1071ef:	eb c4                	jmp    1071b5 <ramfs_readdir+0xb5>
  1071f1:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1071f8:	00 
  1071f9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00107200 <ramfs_write>:
  107200:	55                   	push   %ebp
  107201:	57                   	push   %edi
  107202:	56                   	push   %esi
  107203:	53                   	push   %ebx
  107204:	83 ec 1c             	sub    $0x1c,%esp
  107207:	8b 5c 24 38          	mov    0x38(%esp),%ebx
  10720b:	8b 74 24 34          	mov    0x34(%esp),%esi
  10720f:	8b 7c 24 30          	mov    0x30(%esp),%edi
  107213:	01 de                	add    %ebx,%esi
  107215:	8b af 28 01 00 00    	mov    0x128(%edi),%ebp
  10721b:	81 fe 00 00 00 01    	cmp    $0x1000000,%esi
  107221:	77 7d                	ja     1072a0 <ramfs_write+0xa0>
  107223:	8b 85 34 01 00 00    	mov    0x134(%ebp),%eax
  107229:	39 b5 38 01 00 00    	cmp    %esi,0x138(%ebp)
  10722f:	73 31                	jae    107262 <ramfs_write+0x62>
  107231:	8d 8e ff 0f 00 00    	lea    0xfff(%esi),%ecx
  107237:	83 ec 08             	sub    $0x8,%esp
  10723a:	81 e1 00 f0 ff ff    	and    $0xfffff000,%ecx
  107240:	51                   	push   %ecx
  107241:	89 4c 24 18          	mov    %ecx,0x18(%esp)
  107245:	50                   	push   %eax
  107246:	e8 a5 c3 ff ff       	call   1035f0 <krealloc>
  10724b:	83 c4 10             	add    $0x10,%esp
  10724e:	85 c0                	test   %eax,%eax
  107250:	74 4e                	je     1072a0 <ramfs_write+0xa0>
  107252:	8b 4c 24 0c          	mov    0xc(%esp),%ecx
  107256:	89 85 34 01 00 00    	mov    %eax,0x134(%ebp)
  10725c:	89 8d 38 01 00 00    	mov    %ecx,0x138(%ebp)
  107262:	83 ec 04             	sub    $0x4,%esp
  107265:	53                   	push   %ebx
  107266:	ff 74 24 44          	push   0x44(%esp)
  10726a:	03 44 24 40          	add    0x40(%esp),%eax
  10726e:	50                   	push   %eax
  10726f:	e8 3c a3 ff ff       	call   1015b0 <memcpy>
  107274:	83 c4 10             	add    $0x10,%esp
  107277:	39 b7 08 01 00 00    	cmp    %esi,0x108(%edi)
  10727d:	72 11                	jb     107290 <ramfs_write+0x90>
  10727f:	89 d8                	mov    %ebx,%eax
  107281:	83 c4 1c             	add    $0x1c,%esp
  107284:	5b                   	pop    %ebx
  107285:	5e                   	pop    %esi
  107286:	5f                   	pop    %edi
  107287:	5d                   	pop    %ebp
  107288:	c3                   	ret
  107289:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  107290:	89 b7 08 01 00 00    	mov    %esi,0x108(%edi)
  107296:	eb e7                	jmp    10727f <ramfs_write+0x7f>
  107298:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10729f:	00 
  1072a0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  1072a5:	eb da                	jmp    107281 <ramfs_write+0x81>
  1072a7:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1072ae:	00 
  1072af:	90                   	nop

001072b0 <ramfs_read>:
  1072b0:	56                   	push   %esi
  1072b1:	53                   	push   %ebx
  1072b2:	83 ec 04             	sub    $0x4,%esp
  1072b5:	8b 44 24 10          	mov    0x10(%esp),%eax
  1072b9:	8b 74 24 14          	mov    0x14(%esp),%esi
  1072bd:	8b 90 28 01 00 00    	mov    0x128(%eax),%edx
  1072c3:	8b 8a 34 01 00 00    	mov    0x134(%edx),%ecx
  1072c9:	85 c9                	test   %ecx,%ecx
  1072cb:	74 43                	je     107310 <ramfs_read+0x60>
  1072cd:	8b 90 08 01 00 00    	mov    0x108(%eax),%edx
  1072d3:	31 c0                	xor    %eax,%eax
  1072d5:	39 d6                	cmp    %edx,%esi
  1072d7:	72 07                	jb     1072e0 <ramfs_read+0x30>
  1072d9:	83 c4 04             	add    $0x4,%esp
  1072dc:	5b                   	pop    %ebx
  1072dd:	5e                   	pop    %esi
  1072de:	c3                   	ret
  1072df:	90                   	nop
  1072e0:	29 f2                	sub    %esi,%edx
  1072e2:	3b 54 24 18          	cmp    0x18(%esp),%edx
  1072e6:	89 d3                	mov    %edx,%ebx
  1072e8:	0f 47 5c 24 18       	cmova  0x18(%esp),%ebx
  1072ed:	83 ec 04             	sub    $0x4,%esp
  1072f0:	01 f1                	add    %esi,%ecx
  1072f2:	53                   	push   %ebx
  1072f3:	51                   	push   %ecx
  1072f4:	ff 74 24 28          	push   0x28(%esp)
  1072f8:	e8 b3 a2 ff ff       	call   1015b0 <memcpy>
  1072fd:	83 c4 10             	add    $0x10,%esp
  107300:	89 d8                	mov    %ebx,%eax
  107302:	83 c4 04             	add    $0x4,%esp
  107305:	5b                   	pop    %ebx
  107306:	5e                   	pop    %esi
  107307:	c3                   	ret
  107308:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10730f:	00 
  107310:	83 c4 04             	add    $0x4,%esp
  107313:	31 c0                	xor    %eax,%eax
  107315:	5b                   	pop    %ebx
  107316:	5e                   	pop    %esi
  107317:	c3                   	ret
  107318:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10731f:	00 

00107320 <ramfs_mkdir>:
  107320:	56                   	push   %esi
  107321:	53                   	push   %ebx
  107322:	83 ec 10             	sub    $0x10,%esp
  107325:	8b 44 24 1c          	mov    0x1c(%esp),%eax
  107329:	8b b0 28 01 00 00    	mov    0x128(%eax),%esi
  10732f:	68 48 01 00 00       	push   $0x148
  107334:	e8 c7 c1 ff ff       	call   103500 <kzalloc>
  107339:	83 c4 10             	add    $0x10,%esp
  10733c:	89 c3                	mov    %eax,%ebx
  10733e:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  107343:	85 db                	test   %ebx,%ebx
  107345:	74 5d                	je     1073a4 <ramfs_mkdir+0x84>
  107347:	83 ec 04             	sub    $0x4,%esp
  10734a:	68 ff 00 00 00       	push   $0xff
  10734f:	ff 74 24 1c          	push   0x1c(%esp)
  107353:	53                   	push   %ebx
  107354:	e8 97 a3 ff ff       	call   1016f0 <strncpy>
  107359:	a1 4c b0 10 00       	mov    0x10b04c,%eax
  10735e:	89 9b 28 01 00 00    	mov    %ebx,0x128(%ebx)
  107364:	83 c4 10             	add    $0x10,%esp
  107367:	c7 83 04 01 00 00 02 	movl   $0x2,0x104(%ebx)
  10736e:	00 00 00 
  107371:	89 83 00 01 00 00    	mov    %eax,0x100(%ebx)
  107377:	8d 50 01             	lea    0x1(%eax),%edx
  10737a:	8b 86 3c 01 00 00    	mov    0x13c(%esi),%eax
  107380:	c7 83 24 01 00 00 20 	movl   $0x10b020,0x124(%ebx)
  107387:	b0 10 00 
  10738a:	89 83 40 01 00 00    	mov    %eax,0x140(%ebx)
  107390:	31 c0                	xor    %eax,%eax
  107392:	89 b3 44 01 00 00    	mov    %esi,0x144(%ebx)
  107398:	89 15 4c b0 10 00    	mov    %edx,0x10b04c
  10739e:	89 9e 3c 01 00 00    	mov    %ebx,0x13c(%esi)
  1073a4:	83 c4 04             	add    $0x4,%esp
  1073a7:	5b                   	pop    %ebx
  1073a8:	5e                   	pop    %esi
  1073a9:	c3                   	ret
  1073aa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

001073b0 <ramfs_mount>:
  1073b0:	53                   	push   %ebx
  1073b1:	83 ec 14             	sub    $0x14,%esp
  1073b4:	68 48 01 00 00       	push   $0x148
  1073b9:	e8 42 c1 ff ff       	call   103500 <kzalloc>
  1073be:	83 c4 10             	add    $0x10,%esp
  1073c1:	89 c3                	mov    %eax,%ebx
  1073c3:	85 c0                	test   %eax,%eax
  1073c5:	74 52                	je     107419 <ramfs_mount+0x69>
  1073c7:	83 ec 04             	sub    $0x4,%esp
  1073ca:	68 ff 00 00 00       	push   $0xff
  1073cf:	68 32 95 10 00       	push   $0x109532
  1073d4:	50                   	push   %eax
  1073d5:	e8 16 a3 ff ff       	call   1016f0 <strncpy>
  1073da:	a1 4c b0 10 00       	mov    0x10b04c,%eax
  1073df:	c7 83 04 01 00 00 02 	movl   $0x2,0x104(%ebx)
  1073e6:	00 00 00 
  1073e9:	c7 83 24 01 00 00 20 	movl   $0x10b020,0x124(%ebx)
  1073f0:	b0 10 00 
  1073f3:	8d 50 01             	lea    0x1(%eax),%edx
  1073f6:	89 83 00 01 00 00    	mov    %eax,0x100(%ebx)
  1073fc:	89 9b 28 01 00 00    	mov    %ebx,0x128(%ebx)
  107402:	58                   	pop    %eax
  107403:	89 15 4c b0 10 00    	mov    %edx,0x10b04c
  107409:	5a                   	pop    %edx
  10740a:	68 34 95 10 00       	push   $0x109534
  10740f:	6a 01                	push   $0x1
  107411:	e8 1a d7 ff ff       	call   104b30 <klog>
  107416:	83 c4 10             	add    $0x10,%esp
  107419:	83 c4 08             	add    $0x8,%esp
  10741c:	89 d8                	mov    %ebx,%eax
  10741e:	5b                   	pop    %ebx
  10741f:	c3                   	ret

00107420 <net_init>:
  107420:	83 ec 10             	sub    $0x10,%esp
  107423:	6a 10                	push   $0x10
  107425:	6a 00                	push   $0x0
  107427:	68 24 33 12 00       	push   $0x123324
  10742c:	e8 2f a1 ff ff       	call   101560 <memset>
  107431:	83 c4 0c             	add    $0xc,%esp
  107434:	68 40 01 00 00       	push   $0x140
  107439:	6a 00                	push   $0x0
  10743b:	68 e0 31 12 00       	push   $0x1231e0
  107440:	e8 1b a1 ff ff       	call   101560 <memset>
  107445:	83 c4 0c             	add    $0xc,%esp
  107448:	68 40 81 00 00       	push   $0x8140
  10744d:	6a 00                	push   $0x0
  10744f:	68 a0 b0 11 00       	push   $0x11b0a0
  107454:	e8 07 a1 ff ff       	call   101560 <memset>
  107459:	83 c4 0c             	add    $0xc,%esp
  10745c:	6a 58                	push   $0x58
  10745e:	6a 00                	push   $0x0
  107460:	68 40 b0 11 00       	push   $0x11b040
  107465:	e8 f6 a0 ff ff       	call   101560 <memset>
  10746a:	58                   	pop    %eax
  10746b:	5a                   	pop    %edx
  10746c:	68 ac 9e 10 00       	push   $0x109eac
  107471:	6a 01                	push   $0x1
  107473:	c7 05 20 33 12 00 00 	movl   $0x0,0x123320
  10747a:	00 00 00 
  10747d:	e8 ae d6 ff ff       	call   104b30 <klog>
  107482:	83 c4 1c             	add    $0x1c,%esp
  107485:	c3                   	ret
  107486:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10748d:	00 
  10748e:	66 90                	xchg   %ax,%ax

00107490 <net_register_iface>:
  107490:	83 ec 0c             	sub    $0xc,%esp
  107493:	a1 20 33 12 00       	mov    0x123320,%eax
  107498:	8b 54 24 10          	mov    0x10(%esp),%edx
  10749c:	83 f8 03             	cmp    $0x3,%eax
  10749f:	77 47                	ja     1074e8 <net_register_iface+0x58>
  1074a1:	8d 48 01             	lea    0x1(%eax),%ecx
  1074a4:	89 14 85 24 33 12 00 	mov    %edx,0x123324(,%eax,4)
  1074ab:	8b 42 18             	mov    0x18(%edx),%eax
  1074ae:	83 ec 04             	sub    $0x4,%esp
  1074b1:	89 0d 20 33 12 00    	mov    %ecx,0x123320
  1074b7:	0f b6 c8             	movzbl %al,%ecx
  1074ba:	51                   	push   %ecx
  1074bb:	0f b6 cc             	movzbl %ah,%ecx
  1074be:	51                   	push   %ecx
  1074bf:	89 c1                	mov    %eax,%ecx
  1074c1:	c1 e8 18             	shr    $0x18,%eax
  1074c4:	c1 e9 10             	shr    $0x10,%ecx
  1074c7:	0f b6 c9             	movzbl %cl,%ecx
  1074ca:	51                   	push   %ecx
  1074cb:	50                   	push   %eax
  1074cc:	52                   	push   %edx
  1074cd:	68 d0 9e 10 00       	push   $0x109ed0
  1074d2:	6a 01                	push   $0x1
  1074d4:	e8 57 d6 ff ff       	call   104b30 <klog>
  1074d9:	83 c4 20             	add    $0x20,%esp
  1074dc:	31 c0                	xor    %eax,%eax
  1074de:	83 c4 0c             	add    $0xc,%esp
  1074e1:	c3                   	ret
  1074e2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1074e8:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  1074ed:	eb ef                	jmp    1074de <net_register_iface+0x4e>
  1074ef:	90                   	nop

001074f0 <arp_add>:
  1074f0:	53                   	push   %ebx
  1074f1:	8b 4c 24 08          	mov    0x8(%esp),%ecx
  1074f5:	b8 e0 31 12 00       	mov    $0x1231e0,%eax
  1074fa:	31 d2                	xor    %edx,%edx
  1074fc:	8b 5c 24 0c          	mov    0xc(%esp),%ebx
  107500:	80 78 10 00          	cmpb   $0x0,0x10(%eax)
  107504:	74 04                	je     10750a <arp_add+0x1a>
  107506:	39 08                	cmp    %ecx,(%eax)
  107508:	74 5e                	je     107568 <arp_add+0x78>
  10750a:	83 c2 01             	add    $0x1,%edx
  10750d:	83 c0 14             	add    $0x14,%eax
  107510:	83 fa 10             	cmp    $0x10,%edx
  107513:	75 eb                	jne    107500 <arp_add+0x10>
  107515:	ba f0 31 12 00       	mov    $0x1231f0,%edx
  10751a:	31 c0                	xor    %eax,%eax
  10751c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  107520:	80 3a 00             	cmpb   $0x0,(%edx)
  107523:	74 13                	je     107538 <arp_add+0x48>
  107525:	83 c0 01             	add    $0x1,%eax
  107528:	83 c2 14             	add    $0x14,%edx
  10752b:	83 f8 10             	cmp    $0x10,%eax
  10752e:	75 f0                	jne    107520 <arp_add+0x30>
  107530:	5b                   	pop    %ebx
  107531:	c3                   	ret
  107532:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  107538:	8d 14 80             	lea    (%eax,%eax,4),%edx
  10753b:	c1 e2 02             	shl    $0x2,%edx
  10753e:	8d 82 e0 31 12 00    	lea    0x1231e0(%edx),%eax
  107544:	89 8a e0 31 12 00    	mov    %ecx,0x1231e0(%edx)
  10754a:	8b 13                	mov    (%ebx),%edx
  10754c:	89 50 04             	mov    %edx,0x4(%eax)
  10754f:	0f b7 53 04          	movzwl 0x4(%ebx),%edx
  107553:	c7 40 0c 2c 01 00 00 	movl   $0x12c,0xc(%eax)
  10755a:	66 89 50 08          	mov    %dx,0x8(%eax)
  10755e:	c6 40 10 01          	movb   $0x1,0x10(%eax)
  107562:	5b                   	pop    %ebx
  107563:	c3                   	ret
  107564:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  107568:	8d 04 92             	lea    (%edx,%edx,4),%eax
  10756b:	8b 13                	mov    (%ebx),%edx
  10756d:	8d 04 85 e0 31 12 00 	lea    0x1231e0(,%eax,4),%eax
  107574:	89 50 04             	mov    %edx,0x4(%eax)
  107577:	0f b7 53 04          	movzwl 0x4(%ebx),%edx
  10757b:	c7 40 0c 2c 01 00 00 	movl   $0x12c,0xc(%eax)
  107582:	66 89 50 08          	mov    %dx,0x8(%eax)
  107586:	5b                   	pop    %ebx
  107587:	c3                   	ret
  107588:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10758f:	00 

00107590 <arp_lookup>:
  107590:	8b 4c 24 04          	mov    0x4(%esp),%ecx
  107594:	b8 e0 31 12 00       	mov    $0x1231e0,%eax
  107599:	31 d2                	xor    %edx,%edx
  10759b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  1075a0:	80 78 10 00          	cmpb   $0x0,0x10(%eax)
  1075a4:	74 04                	je     1075aa <arp_lookup+0x1a>
  1075a6:	39 08                	cmp    %ecx,(%eax)
  1075a8:	74 16                	je     1075c0 <arp_lookup+0x30>
  1075aa:	83 c2 01             	add    $0x1,%edx
  1075ad:	83 c0 14             	add    $0x14,%eax
  1075b0:	83 fa 10             	cmp    $0x10,%edx
  1075b3:	75 eb                	jne    1075a0 <arp_lookup+0x10>
  1075b5:	31 c0                	xor    %eax,%eax
  1075b7:	c3                   	ret
  1075b8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1075bf:	00 
  1075c0:	8d 04 92             	lea    (%edx,%edx,4),%eax
  1075c3:	8d 04 85 e4 31 12 00 	lea    0x1231e4(,%eax,4),%eax
  1075ca:	c3                   	ret
  1075cb:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

001075d0 <arp_send_request>:
  1075d0:	56                   	push   %esi
  1075d1:	53                   	push   %ebx
  1075d2:	83 ec 38             	sub    $0x38,%esp
  1075d5:	8b 5c 24 44          	mov    0x44(%esp),%ebx
  1075d9:	6a 2a                	push   $0x2a
  1075db:	6a 00                	push   $0x0
  1075dd:	8d 74 24 10          	lea    0x10(%esp),%esi
  1075e1:	56                   	push   %esi
  1075e2:	e8 79 9f ff ff       	call   101560 <memset>
  1075e7:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  1075ec:	8b 53 10             	mov    0x10(%ebx),%edx
  1075ef:	b9 00 01 00 00       	mov    $0x100,%ecx
  1075f4:	66 89 44 24 18       	mov    %ax,0x18(%esp)
  1075f9:	0f b7 43 14          	movzwl 0x14(%ebx),%eax
  1075fd:	c7 44 24 14 ff ff ff 	movl   $0xffffffff,0x14(%esp)
  107604:	ff 
  107605:	66 89 44 24 1e       	mov    %ax,0x1e(%esp)
  10760a:	66 89 44 24 2e       	mov    %ax,0x2e(%esp)
  10760f:	8b 43 18             	mov    0x18(%ebx),%eax
  107612:	89 54 24 1a          	mov    %edx,0x1a(%esp)
  107616:	0f c8                	bswap  %eax
  107618:	89 44 24 30          	mov    %eax,0x30(%esp)
  10761c:	8b 44 24 54          	mov    0x54(%esp),%eax
  107620:	c7 44 24 20 08 06 00 	movl   $0x1000608,0x20(%esp)
  107627:	01 
  107628:	0f c8                	bswap  %eax
  10762a:	89 44 24 3a          	mov    %eax,0x3a(%esp)
  10762e:	8b 43 28             	mov    0x28(%ebx),%eax
  107631:	c7 44 24 24 08 00 06 	movl   $0x4060008,0x24(%esp)
  107638:	04 
  107639:	66 89 4c 24 28       	mov    %cx,0x28(%esp)
  10763e:	89 54 24 2a          	mov    %edx,0x2a(%esp)
  107642:	83 c4 10             	add    $0x10,%esp
  107645:	85 c0                	test   %eax,%eax
  107647:	74 0c                	je     107655 <arp_send_request+0x85>
  107649:	83 ec 04             	sub    $0x4,%esp
  10764c:	6a 2a                	push   $0x2a
  10764e:	56                   	push   %esi
  10764f:	53                   	push   %ebx
  107650:	ff d0                	call   *%eax
  107652:	83 c4 10             	add    $0x10,%esp
  107655:	83 05 68 b0 11 00 01 	addl   $0x1,0x11b068
  10765c:	83 15 6c b0 11 00 00 	adcl   $0x0,0x11b06c
  107663:	83 c4 34             	add    $0x34,%esp
  107666:	5b                   	pop    %ebx
  107667:	5e                   	pop    %esi
  107668:	c3                   	ret
  107669:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00107670 <ip4_send>:
  107670:	55                   	push   %ebp
  107671:	57                   	push   %edi
  107672:	56                   	push   %esi
  107673:	53                   	push   %ebx
  107674:	83 ec 1c             	sub    $0x1c,%esp
  107677:	8b 44 24 38          	mov    0x38(%esp),%eax
  10767b:	8b 7c 24 30          	mov    0x30(%esp),%edi
  10767f:	89 44 24 0c          	mov    %eax,0xc(%esp)
  107683:	85 ff                	test   %edi,%edi
  107685:	0f 84 8d 01 00 00    	je     107818 <ip4_send+0x1a8>
  10768b:	8b 5f 28             	mov    0x28(%edi),%ebx
  10768e:	85 db                	test   %ebx,%ebx
  107690:	0f 84 82 01 00 00    	je     107818 <ip4_send+0x1a8>
  107696:	8b 44 24 40          	mov    0x40(%esp),%eax
  10769a:	83 ec 0c             	sub    $0xc,%esp
  10769d:	8d 70 22             	lea    0x22(%eax),%esi
  1076a0:	56                   	push   %esi
  1076a1:	e8 fa bc ff ff       	call   1033a0 <kmalloc>
  1076a6:	83 c4 10             	add    $0x10,%esp
  1076a9:	89 c3                	mov    %eax,%ebx
  1076ab:	85 c0                	test   %eax,%eax
  1076ad:	0f 84 65 01 00 00    	je     107818 <ip4_send+0x1a8>
  1076b3:	8b 47 18             	mov    0x18(%edi),%eax
  1076b6:	8b 4c 24 34          	mov    0x34(%esp),%ecx
  1076ba:	89 c5                	mov    %eax,%ebp
  1076bc:	33 44 24 34          	xor    0x34(%esp),%eax
  1076c0:	23 47 1c             	and    0x1c(%edi),%eax
  1076c3:	0f cd                	bswap  %ebp
  1076c5:	0f 85 45 01 00 00    	jne    107810 <ip4_send+0x1a0>
  1076cb:	b8 e0 31 12 00       	mov    $0x1231e0,%eax
  1076d0:	31 d2                	xor    %edx,%edx
  1076d2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1076d8:	80 78 10 00          	cmpb   $0x0,0x10(%eax)
  1076dc:	74 04                	je     1076e2 <ip4_send+0x72>
  1076de:	3b 08                	cmp    (%eax),%ecx
  1076e0:	74 2e                	je     107710 <ip4_send+0xa0>
  1076e2:	83 c2 01             	add    $0x1,%edx
  1076e5:	83 c0 14             	add    $0x14,%eax
  1076e8:	83 fa 10             	cmp    $0x10,%edx
  1076eb:	75 eb                	jne    1076d8 <ip4_send+0x68>
  1076ed:	83 ec 08             	sub    $0x8,%esp
  1076f0:	51                   	push   %ecx
  1076f1:	57                   	push   %edi
  1076f2:	bf fe ff ff ff       	mov    $0xfffffffe,%edi
  1076f7:	e8 d4 fe ff ff       	call   1075d0 <arp_send_request>
  1076fc:	89 1c 24             	mov    %ebx,(%esp)
  1076ff:	e8 5c be ff ff       	call   103560 <kfree>
  107704:	83 c4 10             	add    $0x10,%esp
  107707:	e9 f1 00 00 00       	jmp    1077fd <ip4_send+0x18d>
  10770c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  107710:	8d 04 92             	lea    (%edx,%edx,4),%eax
  107713:	0f b7 4c 24 40       	movzwl 0x40(%esp),%ecx
  107718:	8d 04 85 e0 31 12 00 	lea    0x1231e0(,%eax,4),%eax
  10771f:	8b 50 04             	mov    0x4(%eax),%edx
  107722:	89 13                	mov    %edx,(%ebx)
  107724:	0f b7 40 08          	movzwl 0x8(%eax),%eax
  107728:	8d 51 14             	lea    0x14(%ecx),%edx
  10772b:	66 c1 c2 08          	rol    $0x8,%dx
  10772f:	66 89 43 04          	mov    %ax,0x4(%ebx)
  107733:	8b 47 10             	mov    0x10(%edi),%eax
  107736:	89 43 06             	mov    %eax,0x6(%ebx)
  107739:	0f b7 47 14          	movzwl 0x14(%edi),%eax
  10773d:	66 89 53 10          	mov    %dx,0x10(%ebx)
  107741:	0f b7 15 50 b0 10 00 	movzwl 0x10b050,%edx
  107748:	c7 43 0c 08 00 45 00 	movl   $0x450008,0xc(%ebx)
  10774f:	8d 4a 01             	lea    0x1(%edx),%ecx
  107752:	66 c1 c2 08          	rol    $0x8,%dx
  107756:	c6 43 16 40          	movb   $0x40,0x16(%ebx)
  10775a:	66 89 0d 50 b0 10 00 	mov    %cx,0x10b050
  107761:	0f b6 4c 24 0c       	movzbl 0xc(%esp),%ecx
  107766:	66 89 53 12          	mov    %dx,0x12(%ebx)
  10776a:	31 d2                	xor    %edx,%edx
  10776c:	88 4b 17             	mov    %cl,0x17(%ebx)
  10776f:	31 c9                	xor    %ecx,%ecx
  107771:	66 89 53 14          	mov    %dx,0x14(%ebx)
  107775:	8b 54 24 34          	mov    0x34(%esp),%edx
  107779:	66 89 4b 18          	mov    %cx,0x18(%ebx)
  10777d:	0f ca                	bswap  %edx
  10777f:	66 89 43 0a          	mov    %ax,0xa(%ebx)
  107783:	8d 43 0e             	lea    0xe(%ebx),%eax
  107786:	89 6b 1a             	mov    %ebp,0x1a(%ebx)
  107789:	8d 6b 22             	lea    0x22(%ebx),%ebp
  10778c:	89 53 1e             	mov    %edx,0x1e(%ebx)
  10778f:	31 d2                	xor    %edx,%edx
  107791:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  107798:	0f b7 08             	movzwl (%eax),%ecx
  10779b:	83 c0 02             	add    $0x2,%eax
  10779e:	01 ca                	add    %ecx,%edx
  1077a0:	39 c5                	cmp    %eax,%ebp
  1077a2:	75 f4                	jne    107798 <ip4_send+0x128>
  1077a4:	89 d0                	mov    %edx,%eax
  1077a6:	0f b7 d2             	movzwl %dx,%edx
  1077a9:	83 ec 04             	sub    $0x4,%esp
  1077ac:	c1 e8 10             	shr    $0x10,%eax
  1077af:	01 d0                	add    %edx,%eax
  1077b1:	89 c2                	mov    %eax,%edx
  1077b3:	c1 ea 10             	shr    $0x10,%edx
  1077b6:	01 d0                	add    %edx,%eax
  1077b8:	f7 d0                	not    %eax
  1077ba:	66 89 43 18          	mov    %ax,0x18(%ebx)
  1077be:	ff 74 24 44          	push   0x44(%esp)
  1077c2:	ff 74 24 44          	push   0x44(%esp)
  1077c6:	55                   	push   %ebp
  1077c7:	e8 e4 9d ff ff       	call   1015b0 <memcpy>
  1077cc:	83 c4 0c             	add    $0xc,%esp
  1077cf:	56                   	push   %esi
  1077d0:	53                   	push   %ebx
  1077d1:	57                   	push   %edi
  1077d2:	ff 57 28             	call   *0x28(%edi)
  1077d5:	89 c7                	mov    %eax,%edi
  1077d7:	89 1c 24             	mov    %ebx,(%esp)
  1077da:	e8 81 bd ff ff       	call   103560 <kfree>
  1077df:	83 05 40 b0 11 00 01 	addl   $0x1,0x11b040
  1077e6:	83 15 44 b0 11 00 00 	adcl   $0x0,0x11b044
  1077ed:	01 35 48 b0 11 00    	add    %esi,0x11b048
  1077f3:	83 15 4c b0 11 00 00 	adcl   $0x0,0x11b04c
  1077fa:	83 c4 10             	add    $0x10,%esp
  1077fd:	83 c4 1c             	add    $0x1c,%esp
  107800:	89 f8                	mov    %edi,%eax
  107802:	5b                   	pop    %ebx
  107803:	5e                   	pop    %esi
  107804:	5f                   	pop    %edi
  107805:	5d                   	pop    %ebp
  107806:	c3                   	ret
  107807:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10780e:	00 
  10780f:	90                   	nop
  107810:	8b 4f 20             	mov    0x20(%edi),%ecx
  107813:	e9 b3 fe ff ff       	jmp    1076cb <ip4_send+0x5b>
  107818:	bf ff ff ff ff       	mov    $0xffffffff,%edi
  10781d:	eb de                	jmp    1077fd <ip4_send+0x18d>
  10781f:	90                   	nop

00107820 <icmp_ping>:
  107820:	56                   	push   %esi
  107821:	53                   	push   %ebx
  107822:	83 ec 38             	sub    $0x38,%esp
  107825:	8b 74 24 4c          	mov    0x4c(%esp),%esi
  107829:	6a 28                	push   $0x28
  10782b:	68 ab 00 00 00       	push   $0xab
  107830:	66 c1 c6 08          	rol    $0x8,%si
  107834:	8d 5c 24 14          	lea    0x14(%esp),%ebx
  107838:	53                   	push   %ebx
  107839:	e8 22 9d ff ff       	call   101560 <memset>
  10783e:	b8 12 34 00 00       	mov    $0x3412,%eax
  107843:	66 89 74 24 1e       	mov    %si,0x1e(%esp)
  107848:	31 d2                	xor    %edx,%edx
  10784a:	66 89 44 24 1c       	mov    %ax,0x1c(%esp)
  10784f:	89 d8                	mov    %ebx,%eax
  107851:	c7 44 24 18 08 00 00 	movl   $0x8,0x18(%esp)
  107858:	00 
  107859:	83 c4 10             	add    $0x10,%esp
  10785c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  107860:	0f b7 08             	movzwl (%eax),%ecx
  107863:	83 c0 02             	add    $0x2,%eax
  107866:	8d 74 24 30          	lea    0x30(%esp),%esi
  10786a:	01 ca                	add    %ecx,%edx
  10786c:	39 f0                	cmp    %esi,%eax
  10786e:	75 f0                	jne    107860 <icmp_ping+0x40>
  107870:	89 d0                	mov    %edx,%eax
  107872:	0f b7 d2             	movzwl %dx,%edx
  107875:	83 ec 0c             	sub    $0xc,%esp
  107878:	c1 e8 10             	shr    $0x10,%eax
  10787b:	01 d0                	add    %edx,%eax
  10787d:	89 c2                	mov    %eax,%edx
  10787f:	c1 ea 10             	shr    $0x10,%edx
  107882:	01 d0                	add    %edx,%eax
  107884:	f7 d0                	not    %eax
  107886:	66 89 44 24 16       	mov    %ax,0x16(%esp)
  10788b:	6a 28                	push   $0x28
  10788d:	53                   	push   %ebx
  10788e:	6a 01                	push   $0x1
  107890:	ff 74 24 5c          	push   0x5c(%esp)
  107894:	ff 74 24 5c          	push   0x5c(%esp)
  107898:	e8 d3 fd ff ff       	call   107670 <ip4_send>
  10789d:	83 c4 54             	add    $0x54,%esp
  1078a0:	5b                   	pop    %ebx
  1078a1:	5e                   	pop    %esi
  1078a2:	c3                   	ret
  1078a3:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1078aa:	00 
  1078ab:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

001078b0 <udp_send>:
  1078b0:	55                   	push   %ebp
  1078b1:	57                   	push   %edi
  1078b2:	56                   	push   %esi
  1078b3:	53                   	push   %ebx
  1078b4:	83 ec 18             	sub    $0x18,%esp
  1078b7:	8b 44 24 40          	mov    0x40(%esp),%eax
  1078bb:	8b 7c 24 34          	mov    0x34(%esp),%edi
  1078bf:	8b 74 24 38          	mov    0x38(%esp),%esi
  1078c3:	8d 58 08             	lea    0x8(%eax),%ebx
  1078c6:	53                   	push   %ebx
  1078c7:	e8 d4 ba ff ff       	call   1033a0 <kmalloc>
  1078cc:	83 c4 10             	add    $0x10,%esp
  1078cf:	85 c0                	test   %eax,%eax
  1078d1:	74 6e                	je     107941 <udp_send+0x91>
  1078d3:	66 c1 c7 08          	rol    $0x8,%di
  1078d7:	66 c1 c6 08          	rol    $0x8,%si
  1078db:	89 c5                	mov    %eax,%ebp
  1078dd:	83 ec 04             	sub    $0x4,%esp
  1078e0:	66 89 38             	mov    %di,(%eax)
  1078e3:	66 89 70 02          	mov    %si,0x2(%eax)
  1078e7:	89 d8                	mov    %ebx,%eax
  1078e9:	66 c1 c0 08          	rol    $0x8,%ax
  1078ed:	66 89 45 04          	mov    %ax,0x4(%ebp)
  1078f1:	31 c0                	xor    %eax,%eax
  1078f3:	66 89 45 06          	mov    %ax,0x6(%ebp)
  1078f7:	8d 45 08             	lea    0x8(%ebp),%eax
  1078fa:	ff 74 24 38          	push   0x38(%esp)
  1078fe:	ff 74 24 38          	push   0x38(%esp)
  107902:	50                   	push   %eax
  107903:	e8 a8 9c ff ff       	call   1015b0 <memcpy>
  107908:	89 1c 24             	mov    %ebx,(%esp)
  10790b:	55                   	push   %ebp
  10790c:	6a 11                	push   $0x11
  10790e:	ff 74 24 3c          	push   0x3c(%esp)
  107912:	ff 74 24 3c          	push   0x3c(%esp)
  107916:	e8 55 fd ff ff       	call   107670 <ip4_send>
  10791b:	83 c4 14             	add    $0x14,%esp
  10791e:	55                   	push   %ebp
  10791f:	89 c3                	mov    %eax,%ebx
  107921:	e8 3a bc ff ff       	call   103560 <kfree>
  107926:	83 05 88 b0 11 00 01 	addl   $0x1,0x11b088
  10792d:	83 15 8c b0 11 00 00 	adcl   $0x0,0x11b08c
  107934:	83 c4 10             	add    $0x10,%esp
  107937:	83 c4 0c             	add    $0xc,%esp
  10793a:	89 d8                	mov    %ebx,%eax
  10793c:	5b                   	pop    %ebx
  10793d:	5e                   	pop    %esi
  10793e:	5f                   	pop    %edi
  10793f:	5d                   	pop    %ebp
  107940:	c3                   	ret
  107941:	bb ff ff ff ff       	mov    $0xffffffff,%ebx
  107946:	eb ef                	jmp    107937 <udp_send+0x87>
  107948:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10794f:	00 

00107950 <net_receive>:
  107950:	55                   	push   %ebp
  107951:	57                   	push   %edi
  107952:	56                   	push   %esi
  107953:	53                   	push   %ebx
  107954:	83 ec 5c             	sub    $0x5c,%esp
  107957:	8b 44 24 78          	mov    0x78(%esp),%eax
  10795b:	8b 5c 24 74          	mov    0x74(%esp),%ebx
  10795f:	83 f8 0d             	cmp    $0xd,%eax
  107962:	76 3c                	jbe    1079a0 <net_receive+0x50>
  107964:	0f b7 53 0c          	movzwl 0xc(%ebx),%edx
  107968:	83 05 50 b0 11 00 01 	addl   $0x1,0x11b050
  10796f:	83 15 54 b0 11 00 00 	adcl   $0x0,0x11b054
  107976:	01 05 58 b0 11 00    	add    %eax,0x11b058
  10797c:	83 15 5c b0 11 00 00 	adcl   $0x0,0x11b05c
  107983:	83 e8 0e             	sub    $0xe,%eax
  107986:	66 83 fa 08          	cmp    $0x8,%dx
  10798a:	74 24                	je     1079b0 <net_receive+0x60>
  10798c:	66 81 fa 08 06       	cmp    $0x608,%dx
  107991:	0f 85 59 01 00 00    	jne    107af0 <net_receive+0x1a0>
  107997:	83 f8 1b             	cmp    $0x1b,%eax
  10799a:	0f 87 70 01 00 00    	ja     107b10 <net_receive+0x1c0>
  1079a0:	83 c4 5c             	add    $0x5c,%esp
  1079a3:	5b                   	pop    %ebx
  1079a4:	5e                   	pop    %esi
  1079a5:	5f                   	pop    %edi
  1079a6:	5d                   	pop    %ebp
  1079a7:	c3                   	ret
  1079a8:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1079af:	00 
  1079b0:	83 f8 13             	cmp    $0x13,%eax
  1079b3:	76 eb                	jbe    1079a0 <net_receive+0x50>
  1079b5:	0f b7 4b 10          	movzwl 0x10(%ebx),%ecx
  1079b9:	0f b6 53 0e          	movzbl 0xe(%ebx),%edx
  1079bd:	66 c1 c1 08          	rol    $0x8,%cx
  1079c1:	83 e2 0f             	and    $0xf,%edx
  1079c4:	0f b7 c9             	movzwl %cx,%ecx
  1079c7:	c1 e2 02             	shl    $0x2,%edx
  1079ca:	39 c8                	cmp    %ecx,%eax
  1079cc:	72 d2                	jb     1079a0 <net_receive+0x50>
  1079ce:	83 fa 13             	cmp    $0x13,%edx
  1079d1:	76 cd                	jbe    1079a0 <net_receive+0x50>
  1079d3:	8b 43 1e             	mov    0x1e(%ebx),%eax
  1079d6:	8b 74 24 70          	mov    0x70(%esp),%esi
  1079da:	29 d1                	sub    %edx,%ecx
  1079dc:	8d 6c 13 0e          	lea    0xe(%ebx,%edx,1),%ebp
  1079e0:	89 4c 24 0c          	mov    %ecx,0xc(%esp)
  1079e4:	8b 56 18             	mov    0x18(%esi),%edx
  1079e7:	83 f8 ff             	cmp    $0xffffffff,%eax
  1079ea:	89 c7                	mov    %eax,%edi
  1079ec:	0f 95 c0             	setne  %al
  1079ef:	0f cf                	bswap  %edi
  1079f1:	89 c6                	mov    %eax,%esi
  1079f3:	39 d7                	cmp    %edx,%edi
  1079f5:	0f 95 c0             	setne  %al
  1079f8:	89 f1                	mov    %esi,%ecx
  1079fa:	84 c1                	test   %al,%cl
  1079fc:	74 04                	je     107a02 <net_receive+0xb2>
  1079fe:	85 d2                	test   %edx,%edx
  107a00:	75 9e                	jne    1079a0 <net_receive+0x50>
  107a02:	0f b6 43 17          	movzbl 0x17(%ebx),%eax
  107a06:	3c 01                	cmp    $0x1,%al
  107a08:	0f 84 ca 01 00 00    	je     107bd8 <net_receive+0x288>
  107a0e:	3c 11                	cmp    $0x11,%al
  107a10:	75 8e                	jne    1079a0 <net_receive+0x50>
  107a12:	83 7c 24 0c 07       	cmpl   $0x7,0xc(%esp)
  107a17:	76 87                	jbe    1079a0 <net_receive+0x50>
  107a19:	0f b7 45 02          	movzwl 0x2(%ebp),%eax
  107a1d:	31 db                	xor    %ebx,%ebx
  107a1f:	89 6c 24 1c          	mov    %ebp,0x1c(%esp)
  107a23:	89 5c 24 0c          	mov    %ebx,0xc(%esp)
  107a27:	66 c1 c0 08          	rol    $0x8,%ax
  107a2b:	66 89 44 24 16       	mov    %ax,0x16(%esp)
  107a30:	0f b7 45 04          	movzwl 0x4(%ebp),%eax
  107a34:	66 c1 c0 08          	rol    $0x8,%ax
  107a38:	0f b7 c0             	movzwl %ax,%eax
  107a3b:	89 44 24 18          	mov    %eax,0x18(%esp)
  107a3f:	b8 a0 b0 11 00       	mov    $0x11b0a0,%eax
  107a44:	eb 21                	jmp    107a67 <net_receive+0x117>
  107a46:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  107a4d:	00 
  107a4e:	66 90                	xchg   %ax,%ax
  107a50:	83 44 24 0c 01       	addl   $0x1,0xc(%esp)
  107a55:	8b 7c 24 0c          	mov    0xc(%esp),%edi
  107a59:	05 14 08 00 00       	add    $0x814,%eax
  107a5e:	83 ff 10             	cmp    $0x10,%edi
  107a61:	0f 84 39 ff ff ff    	je     1079a0 <net_receive+0x50>
  107a67:	80 38 00             	cmpb   $0x0,(%eax)
  107a6a:	74 e4                	je     107a50 <net_receive+0x100>
  107a6c:	80 78 01 11          	cmpb   $0x11,0x1(%eax)
  107a70:	75 de                	jne    107a50 <net_receive+0x100>
  107a72:	0f b7 74 24 16       	movzwl 0x16(%esp),%esi
  107a77:	66 3b 70 02          	cmp    0x2(%eax),%si
  107a7b:	75 d3                	jne    107a50 <net_receive+0x100>
  107a7d:	83 7c 24 18 08       	cmpl   $0x8,0x18(%esp)
  107a82:	74 53                	je     107ad7 <net_receive+0x187>
  107a84:	69 5c 24 0c 14 08 00 	imul   $0x814,0xc(%esp),%ebx
  107a8b:	00 
  107a8c:	8b 74 24 1c          	mov    0x1c(%esp),%esi
  107a90:	89 44 24 10          	mov    %eax,0x10(%esp)
  107a94:	8b 7c 24 18          	mov    0x18(%esp),%edi
  107a98:	8b a8 0c 08 00 00    	mov    0x80c(%eax),%ebp
  107a9e:	8b 90 10 08 00 00    	mov    0x810(%eax),%edx
  107aa4:	8d 4e 08             	lea    0x8(%esi),%ecx
  107aa7:	01 f7                	add    %esi,%edi
  107aa9:	8d b3 a0 b0 11 00    	lea    0x11b0a0(%ebx),%esi
  107aaf:	90                   	nop
  107ab0:	89 d3                	mov    %edx,%ebx
  107ab2:	8d 52 01             	lea    0x1(%edx),%edx
  107ab5:	81 e2 ff 07 00 00    	and    $0x7ff,%edx
  107abb:	39 ea                	cmp    %ebp,%edx
  107abd:	74 07                	je     107ac6 <net_receive+0x176>
  107abf:	0f b6 01             	movzbl (%ecx),%eax
  107ac2:	88 44 1e 0a          	mov    %al,0xa(%esi,%ebx,1)
  107ac6:	8b 44 24 10          	mov    0x10(%esp),%eax
  107aca:	83 c1 01             	add    $0x1,%ecx
  107acd:	89 90 10 08 00 00    	mov    %edx,0x810(%eax)
  107ad3:	39 f9                	cmp    %edi,%ecx
  107ad5:	75 d9                	jne    107ab0 <net_receive+0x160>
  107ad7:	83 05 90 b0 11 00 01 	addl   $0x1,0x11b090
  107ade:	83 15 94 b0 11 00 00 	adcl   $0x0,0x11b094
  107ae5:	e9 66 ff ff ff       	jmp    107a50 <net_receive+0x100>
  107aea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  107af0:	83 05 60 b0 11 00 01 	addl   $0x1,0x11b060
  107af7:	83 15 64 b0 11 00 00 	adcl   $0x0,0x11b064
  107afe:	83 c4 5c             	add    $0x5c,%esp
  107b01:	5b                   	pop    %ebx
  107b02:	5e                   	pop    %esi
  107b03:	5f                   	pop    %edi
  107b04:	5d                   	pop    %ebp
  107b05:	c3                   	ret
  107b06:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  107b0d:	00 
  107b0e:	66 90                	xchg   %ax,%ax
  107b10:	8b 73 1c             	mov    0x1c(%ebx),%esi
  107b13:	8d 53 16             	lea    0x16(%ebx),%edx
  107b16:	52                   	push   %edx
  107b17:	89 f0                	mov    %esi,%eax
  107b19:	0f c8                	bswap  %eax
  107b1b:	50                   	push   %eax
  107b1c:	e8 cf f9 ff ff       	call   1074f0 <arp_add>
  107b21:	66 81 7b 14 00 01    	cmpw   $0x100,0x14(%ebx)
  107b27:	58                   	pop    %eax
  107b28:	5a                   	pop    %edx
  107b29:	0f 85 71 fe ff ff    	jne    1079a0 <net_receive+0x50>
  107b2f:	8b 4c 24 70          	mov    0x70(%esp),%ecx
  107b33:	8b 43 26             	mov    0x26(%ebx),%eax
  107b36:	8b 51 18             	mov    0x18(%ecx),%edx
  107b39:	0f c8                	bswap  %eax
  107b3b:	39 c2                	cmp    %eax,%edx
  107b3d:	0f 85 5d fe ff ff    	jne    1079a0 <net_receive+0x50>
  107b43:	0f b7 43 1a          	movzwl 0x1a(%ebx),%eax
  107b47:	8b 4b 16             	mov    0x16(%ebx),%ecx
  107b4a:	bd 00 02 00 00       	mov    $0x200,%ebp
  107b4f:	0f ca                	bswap  %edx
  107b51:	8b 7c 24 70          	mov    0x70(%esp),%edi
  107b55:	8b 5c 24 70          	mov    0x70(%esp),%ebx
  107b59:	c7 44 24 30 08 06 00 	movl   $0x1000608,0x30(%esp)
  107b60:	01 
  107b61:	66 89 44 24 28       	mov    %ax,0x28(%esp)
  107b66:	8b 7f 10             	mov    0x10(%edi),%edi
  107b69:	0f b7 5b 14          	movzwl 0x14(%ebx),%ebx
  107b6d:	89 4c 24 24          	mov    %ecx,0x24(%esp)
  107b71:	66 89 44 24 48       	mov    %ax,0x48(%esp)
  107b76:	8b 44 24 70          	mov    0x70(%esp),%eax
  107b7a:	89 7c 24 2a          	mov    %edi,0x2a(%esp)
  107b7e:	8b 40 28             	mov    0x28(%eax),%eax
  107b81:	66 89 5c 24 2e       	mov    %bx,0x2e(%esp)
  107b86:	c7 44 24 34 08 00 06 	movl   $0x4060008,0x34(%esp)
  107b8d:	04 
  107b8e:	66 89 6c 24 38       	mov    %bp,0x38(%esp)
  107b93:	89 7c 24 3a          	mov    %edi,0x3a(%esp)
  107b97:	66 89 5c 24 3e       	mov    %bx,0x3e(%esp)
  107b9c:	89 54 24 40          	mov    %edx,0x40(%esp)
  107ba0:	89 4c 24 44          	mov    %ecx,0x44(%esp)
  107ba4:	89 74 24 4a          	mov    %esi,0x4a(%esp)
  107ba8:	85 c0                	test   %eax,%eax
  107baa:	74 13                	je     107bbf <net_receive+0x26f>
  107bac:	83 ec 04             	sub    $0x4,%esp
  107baf:	6a 2a                	push   $0x2a
  107bb1:	8d 54 24 2c          	lea    0x2c(%esp),%edx
  107bb5:	52                   	push   %edx
  107bb6:	ff 74 24 7c          	push   0x7c(%esp)
  107bba:	ff d0                	call   *%eax
  107bbc:	83 c4 10             	add    $0x10,%esp
  107bbf:	83 05 70 b0 11 00 01 	addl   $0x1,0x11b070
  107bc6:	83 15 74 b0 11 00 00 	adcl   $0x0,0x11b074
  107bcd:	e9 ce fd ff ff       	jmp    1079a0 <net_receive+0x50>
  107bd2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  107bd8:	83 7c 24 0c 07       	cmpl   $0x7,0xc(%esp)
  107bdd:	0f 86 bd fd ff ff    	jbe    1079a0 <net_receive+0x50>
  107be3:	80 7d 00 08          	cmpb   $0x8,0x0(%ebp)
  107be7:	0f 85 b3 fd ff ff    	jne    1079a0 <net_receive+0x50>
  107bed:	83 05 78 b0 11 00 01 	addl   $0x1,0x11b078
  107bf4:	83 15 7c b0 11 00 00 	adcl   $0x0,0x11b07c
  107bfb:	83 ec 0c             	sub    $0xc,%esp
  107bfe:	8b 7c 24 18          	mov    0x18(%esp),%edi
  107c02:	57                   	push   %edi
  107c03:	e8 98 b7 ff ff       	call   1033a0 <kmalloc>
  107c08:	83 c4 10             	add    $0x10,%esp
  107c0b:	89 c6                	mov    %eax,%esi
  107c0d:	85 c0                	test   %eax,%eax
  107c0f:	0f 84 8b fd ff ff    	je     1079a0 <net_receive+0x50>
  107c15:	83 ec 04             	sub    $0x4,%esp
  107c18:	57                   	push   %edi
  107c19:	55                   	push   %ebp
  107c1a:	50                   	push   %eax
  107c1b:	e8 90 99 ff ff       	call   1015b0 <memcpy>
  107c20:	31 c0                	xor    %eax,%eax
  107c22:	c6 06 00             	movb   $0x0,(%esi)
  107c25:	89 f9                	mov    %edi,%ecx
  107c27:	66 89 46 02          	mov    %ax,0x2(%esi)
  107c2b:	8d 47 fe             	lea    -0x2(%edi),%eax
  107c2e:	83 c4 10             	add    $0x10,%esp
  107c31:	31 d2                	xor    %edx,%edx
  107c33:	d1 e8                	shr    $1,%eax
  107c35:	8d 6c 46 02          	lea    0x2(%esi,%eax,2),%ebp
  107c39:	89 f0                	mov    %esi,%eax
  107c3b:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi
  107c40:	0f b7 38             	movzwl (%eax),%edi
  107c43:	83 c0 02             	add    $0x2,%eax
  107c46:	01 fa                	add    %edi,%edx
  107c48:	39 c5                	cmp    %eax,%ebp
  107c4a:	75 f4                	jne    107c40 <net_receive+0x2f0>
  107c4c:	89 4c 24 0c          	mov    %ecx,0xc(%esp)
  107c50:	83 e1 01             	and    $0x1,%ecx
  107c53:	74 06                	je     107c5b <net_receive+0x30b>
  107c55:	0f b6 45 00          	movzbl 0x0(%ebp),%eax
  107c59:	01 c2                	add    %eax,%edx
  107c5b:	89 d0                	mov    %edx,%eax
  107c5d:	0f b7 d2             	movzwl %dx,%edx
  107c60:	83 ec 0c             	sub    $0xc,%esp
  107c63:	c1 e8 10             	shr    $0x10,%eax
  107c66:	01 d0                	add    %edx,%eax
  107c68:	89 c2                	mov    %eax,%edx
  107c6a:	c1 ea 10             	shr    $0x10,%edx
  107c6d:	01 d0                	add    %edx,%eax
  107c6f:	f7 d0                	not    %eax
  107c71:	66 89 46 02          	mov    %ax,0x2(%esi)
  107c75:	8b 43 1a             	mov    0x1a(%ebx),%eax
  107c78:	ff 74 24 18          	push   0x18(%esp)
  107c7c:	0f c8                	bswap  %eax
  107c7e:	56                   	push   %esi
  107c7f:	6a 01                	push   $0x1
  107c81:	50                   	push   %eax
  107c82:	ff b4 24 8c 00 00 00 	push   0x8c(%esp)
  107c89:	e8 e2 f9 ff ff       	call   107670 <ip4_send>
  107c8e:	83 c4 14             	add    $0x14,%esp
  107c91:	56                   	push   %esi
  107c92:	e8 c9 b8 ff ff       	call   103560 <kfree>
  107c97:	83 05 80 b0 11 00 01 	addl   $0x1,0x11b080
  107c9e:	83 15 84 b0 11 00 00 	adcl   $0x0,0x11b084
  107ca5:	83 c4 10             	add    $0x10,%esp
  107ca8:	e9 f3 fc ff ff       	jmp    1079a0 <net_receive+0x50>
  107cad:	8d 76 00             	lea    0x0(%esi),%esi

00107cb0 <sock_open>:
  107cb0:	55                   	push   %ebp
  107cb1:	b8 a0 b0 11 00       	mov    $0x11b0a0,%eax
  107cb6:	57                   	push   %edi
  107cb7:	56                   	push   %esi
  107cb8:	53                   	push   %ebx
  107cb9:	31 db                	xor    %ebx,%ebx
  107cbb:	83 ec 0c             	sub    $0xc,%esp
  107cbe:	8b 74 24 20          	mov    0x20(%esp),%esi
  107cc2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  107cc8:	80 38 00             	cmpb   $0x0,(%eax)
  107ccb:	74 23                	je     107cf0 <sock_open+0x40>
  107ccd:	83 c3 01             	add    $0x1,%ebx
  107cd0:	05 14 08 00 00       	add    $0x814,%eax
  107cd5:	83 fb 10             	cmp    $0x10,%ebx
  107cd8:	75 ee                	jne    107cc8 <sock_open+0x18>
  107cda:	83 c4 0c             	add    $0xc,%esp
  107cdd:	bb ff ff ff ff       	mov    $0xffffffff,%ebx
  107ce2:	89 d8                	mov    %ebx,%eax
  107ce4:	5b                   	pop    %ebx
  107ce5:	5e                   	pop    %esi
  107ce6:	5f                   	pop    %edi
  107ce7:	5d                   	pop    %ebp
  107ce8:	c3                   	ret
  107ce9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  107cf0:	69 eb 14 08 00 00    	imul   $0x814,%ebx,%ebp
  107cf6:	83 ec 04             	sub    $0x4,%esp
  107cf9:	68 14 08 00 00       	push   $0x814
  107cfe:	6a 00                	push   $0x0
  107d00:	8d bd a0 b0 11 00    	lea    0x11b0a0(%ebp),%edi
  107d06:	57                   	push   %edi
  107d07:	e8 54 98 ff ff       	call   101560 <memset>
  107d0c:	89 f0                	mov    %esi,%eax
  107d0e:	83 c4 10             	add    $0x10,%esp
  107d11:	c6 85 a0 b0 11 00 01 	movb   $0x1,0x11b0a0(%ebp)
  107d18:	88 85 a1 b0 11 00    	mov    %al,0x11b0a1(%ebp)
  107d1e:	83 c4 0c             	add    $0xc,%esp
  107d21:	89 d8                	mov    %ebx,%eax
  107d23:	5b                   	pop    %ebx
  107d24:	5e                   	pop    %esi
  107d25:	5f                   	pop    %edi
  107d26:	5d                   	pop    %ebp
  107d27:	c3                   	ret
  107d28:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  107d2f:	00 

00107d30 <sock_close>:
  107d30:	8b 44 24 04          	mov    0x4(%esp),%eax
  107d34:	83 f8 0f             	cmp    $0xf,%eax
  107d37:	77 0d                	ja     107d46 <sock_close+0x16>
  107d39:	69 c0 14 08 00 00    	imul   $0x814,%eax,%eax
  107d3f:	c6 80 a0 b0 11 00 00 	movb   $0x0,0x11b0a0(%eax)
  107d46:	c3                   	ret
  107d47:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  107d4e:	00 
  107d4f:	90                   	nop

00107d50 <sock_bind>:
  107d50:	8b 44 24 04          	mov    0x4(%esp),%eax
  107d54:	8b 54 24 08          	mov    0x8(%esp),%edx
  107d58:	83 f8 0f             	cmp    $0xf,%eax
  107d5b:	77 23                	ja     107d80 <sock_bind+0x30>
  107d5d:	69 c0 14 08 00 00    	imul   $0x814,%eax,%eax
  107d63:	80 b8 a0 b0 11 00 00 	cmpb   $0x0,0x11b0a0(%eax)
  107d6a:	74 14                	je     107d80 <sock_bind+0x30>
  107d6c:	66 89 90 a2 b0 11 00 	mov    %dx,0x11b0a2(%eax)
  107d73:	31 c0                	xor    %eax,%eax
  107d75:	c3                   	ret
  107d76:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  107d7d:	00 
  107d7e:	66 90                	xchg   %ax,%ax
  107d80:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  107d85:	c3                   	ret
  107d86:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  107d8d:	00 
  107d8e:	66 90                	xchg   %ax,%ax

00107d90 <sock_sendto>:
  107d90:	83 ec 0c             	sub    $0xc,%esp
  107d93:	8b 44 24 10          	mov    0x10(%esp),%eax
  107d97:	8b 54 24 18          	mov    0x18(%esp),%edx
  107d9b:	83 f8 0f             	cmp    $0xf,%eax
  107d9e:	77 50                	ja     107df0 <sock_sendto+0x60>
  107da0:	69 c0 14 08 00 00    	imul   $0x814,%eax,%eax
  107da6:	80 b8 a0 b0 11 00 00 	cmpb   $0x0,0x11b0a0(%eax)
  107dad:	74 41                	je     107df0 <sock_sendto+0x60>
  107daf:	8b 0d 20 33 12 00    	mov    0x123320,%ecx
  107db5:	85 c9                	test   %ecx,%ecx
  107db7:	74 37                	je     107df0 <sock_sendto+0x60>
  107db9:	0f b7 80 a2 b0 11 00 	movzwl 0x11b0a2(%eax),%eax
  107dc0:	83 ec 08             	sub    $0x8,%esp
  107dc3:	0f b7 d2             	movzwl %dx,%edx
  107dc6:	ff 74 24 28          	push   0x28(%esp)
  107dca:	ff 74 24 28          	push   0x28(%esp)
  107dce:	52                   	push   %edx
  107dcf:	50                   	push   %eax
  107dd0:	ff 74 24 2c          	push   0x2c(%esp)
  107dd4:	ff 35 24 33 12 00    	push   0x123324
  107dda:	e8 d1 fa ff ff       	call   1078b0 <udp_send>
  107ddf:	83 c4 20             	add    $0x20,%esp
  107de2:	83 c4 0c             	add    $0xc,%esp
  107de5:	c3                   	ret
  107de6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  107ded:	00 
  107dee:	66 90                	xchg   %ax,%ax
  107df0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  107df5:	eb eb                	jmp    107de2 <sock_sendto+0x52>
  107df7:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  107dfe:	00 
  107dff:	90                   	nop

00107e00 <sock_recvfrom>:
  107e00:	56                   	push   %esi
  107e01:	53                   	push   %ebx
  107e02:	8b 44 24 0c          	mov    0xc(%esp),%eax
  107e06:	8b 74 24 10          	mov    0x10(%esp),%esi
  107e0a:	8b 5c 24 14          	mov    0x14(%esp),%ebx
  107e0e:	83 f8 0f             	cmp    $0xf,%eax
  107e11:	77 66                	ja     107e79 <sock_recvfrom+0x79>
  107e13:	69 c0 14 08 00 00    	imul   $0x814,%eax,%eax
  107e19:	80 b8 a0 b0 11 00 00 	cmpb   $0x0,0x11b0a0(%eax)
  107e20:	8d 88 a0 b0 11 00    	lea    0x11b0a0(%eax),%ecx
  107e26:	74 51                	je     107e79 <sock_recvfrom+0x79>
  107e28:	85 db                	test   %ebx,%ebx
  107e2a:	74 49                	je     107e75 <sock_recvfrom+0x75>
  107e2c:	8b 91 0c 08 00 00    	mov    0x80c(%ecx),%edx
  107e32:	31 c0                	xor    %eax,%eax
  107e34:	eb 2f                	jmp    107e65 <sock_recvfrom+0x65>
  107e36:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  107e3d:	00 
  107e3e:	66 90                	xchg   %ax,%ax
  107e40:	0f b6 54 11 0a       	movzbl 0xa(%ecx,%edx,1),%edx
  107e45:	83 c0 01             	add    $0x1,%eax
  107e48:	88 54 06 ff          	mov    %dl,-0x1(%esi,%eax,1)
  107e4c:	8b 91 0c 08 00 00    	mov    0x80c(%ecx),%edx
  107e52:	83 c2 01             	add    $0x1,%edx
  107e55:	81 e2 ff 07 00 00    	and    $0x7ff,%edx
  107e5b:	89 91 0c 08 00 00    	mov    %edx,0x80c(%ecx)
  107e61:	39 c3                	cmp    %eax,%ebx
  107e63:	74 0b                	je     107e70 <sock_recvfrom+0x70>
  107e65:	39 91 10 08 00 00    	cmp    %edx,0x810(%ecx)
  107e6b:	75 d3                	jne    107e40 <sock_recvfrom+0x40>
  107e6d:	5b                   	pop    %ebx
  107e6e:	5e                   	pop    %esi
  107e6f:	c3                   	ret
  107e70:	89 d8                	mov    %ebx,%eax
  107e72:	5b                   	pop    %ebx
  107e73:	5e                   	pop    %esi
  107e74:	c3                   	ret
  107e75:	31 c0                	xor    %eax,%eax
  107e77:	eb f4                	jmp    107e6d <sock_recvfrom+0x6d>
  107e79:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  107e7e:	eb ed                	jmp    107e6d <sock_recvfrom+0x6d>

00107e80 <net_get_stats>:
  107e80:	b8 40 b0 11 00       	mov    $0x11b040,%eax
  107e85:	c3                   	ret
  107e86:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  107e8d:	00 
  107e8e:	66 90                	xchg   %ax,%ax

00107e90 <net_dump>:
  107e90:	83 ec 14             	sub    $0x14,%esp
  107e93:	68 4c 95 10 00       	push   $0x10954c
  107e98:	6a 01                	push   $0x1
  107e9a:	e8 91 cc ff ff       	call   104b30 <klog>
  107e9f:	58                   	pop    %eax
  107ea0:	5a                   	pop    %edx
  107ea1:	ff 35 4c b0 11 00    	push   0x11b04c
  107ea7:	ff 35 48 b0 11 00    	push   0x11b048
  107ead:	ff 35 44 b0 11 00    	push   0x11b044
  107eb3:	ff 35 40 b0 11 00    	push   0x11b040
  107eb9:	68 04 9f 10 00       	push   $0x109f04
  107ebe:	6a 01                	push   $0x1
  107ec0:	e8 6b cc ff ff       	call   104b30 <klog>
  107ec5:	83 c4 20             	add    $0x20,%esp
  107ec8:	ff 35 64 b0 11 00    	push   0x11b064
  107ece:	ff 35 60 b0 11 00    	push   0x11b060
  107ed4:	ff 35 5c b0 11 00    	push   0x11b05c
  107eda:	ff 35 58 b0 11 00    	push   0x11b058
  107ee0:	ff 35 54 b0 11 00    	push   0x11b054
  107ee6:	ff 35 50 b0 11 00    	push   0x11b050
  107eec:	68 24 9f 10 00       	push   $0x109f24
  107ef1:	6a 01                	push   $0x1
  107ef3:	e8 38 cc ff ff       	call   104b30 <klog>
  107ef8:	83 c4 18             	add    $0x18,%esp
  107efb:	ff 35 74 b0 11 00    	push   0x11b074
  107f01:	ff 35 70 b0 11 00    	push   0x11b070
  107f07:	ff 35 6c b0 11 00    	push   0x11b06c
  107f0d:	ff 35 68 b0 11 00    	push   0x11b068
  107f13:	68 5e 95 10 00       	push   $0x10955e
  107f18:	6a 01                	push   $0x1
  107f1a:	e8 11 cc ff ff       	call   104b30 <klog>
  107f1f:	83 c4 18             	add    $0x18,%esp
  107f22:	ff 35 84 b0 11 00    	push   0x11b084
  107f28:	ff 35 80 b0 11 00    	push   0x11b080
  107f2e:	ff 35 7c b0 11 00    	push   0x11b07c
  107f34:	ff 35 78 b0 11 00    	push   0x11b078
  107f3a:	68 79 95 10 00       	push   $0x109579
  107f3f:	6a 01                	push   $0x1
  107f41:	e8 ea cb ff ff       	call   104b30 <klog>
  107f46:	83 c4 18             	add    $0x18,%esp
  107f49:	ff 35 94 b0 11 00    	push   0x11b094
  107f4f:	ff 35 90 b0 11 00    	push   0x11b090
  107f55:	ff 35 8c b0 11 00    	push   0x11b08c
  107f5b:	ff 35 88 b0 11 00    	push   0x11b088
  107f61:	68 54 9f 10 00       	push   $0x109f54
  107f66:	6a 01                	push   $0x1
  107f68:	e8 c3 cb ff ff       	call   104b30 <klog>
  107f6d:	83 c4 2c             	add    $0x2c,%esp
  107f70:	c3                   	ret
  107f71:	66 90                	xchg   %ax,%ax
  107f73:	66 90                	xchg   %ax,%ax
  107f75:	66 90                	xchg   %ax,%ax
  107f77:	66 90                	xchg   %ax,%ax
  107f79:	66 90                	xchg   %ax,%ax
  107f7b:	66 90                	xchg   %ax,%ax
  107f7d:	66 90                	xchg   %ax,%ax
  107f7f:	90                   	nop

00107f80 <syscall_handler>:
  107f80:	53                   	push   %ebx
  107f81:	83 ec 08             	sub    $0x8,%esp
  107f84:	8b 5c 24 10          	mov    0x10(%esp),%ebx
  107f88:	8b 43 2c             	mov    0x2c(%ebx),%eax
  107f8b:	3d ff 00 00 00       	cmp    $0xff,%eax
  107f90:	77 2e                	ja     107fc0 <syscall_handler+0x40>
  107f92:	8b 14 85 40 33 12 00 	mov    0x123340(,%eax,4),%edx
  107f99:	85 d2                	test   %edx,%edx
  107f9b:	74 23                	je     107fc0 <syscall_handler+0x40>
  107f9d:	83 ec 0c             	sub    $0xc,%esp
  107fa0:	ff 73 10             	push   0x10(%ebx)
  107fa3:	ff 73 14             	push   0x14(%ebx)
  107fa6:	ff 73 24             	push   0x24(%ebx)
  107fa9:	ff 73 28             	push   0x28(%ebx)
  107fac:	ff 73 20             	push   0x20(%ebx)
  107faf:	ff d2                	call   *%edx
  107fb1:	83 c4 20             	add    $0x20,%esp
  107fb4:	89 43 2c             	mov    %eax,0x2c(%ebx)
  107fb7:	83 c4 08             	add    $0x8,%esp
  107fba:	5b                   	pop    %ebx
  107fbb:	c3                   	ret
  107fbc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  107fc0:	83 ec 04             	sub    $0x4,%esp
  107fc3:	50                   	push   %eax
  107fc4:	68 7c 9f 10 00       	push   $0x109f7c
  107fc9:	6a 02                	push   $0x2
  107fcb:	e8 60 cb ff ff       	call   104b30 <klog>
  107fd0:	83 c4 10             	add    $0x10,%esp
  107fd3:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  107fd8:	89 43 2c             	mov    %eax,0x2c(%ebx)
  107fdb:	83 c4 08             	add    $0x8,%esp
  107fde:	5b                   	pop    %ebx
  107fdf:	c3                   	ret

00107fe0 <sys_yield>:
  107fe0:	83 ec 0c             	sub    $0xc,%esp
  107fe3:	e8 b8 e2 ff ff       	call   1062a0 <task_yield>
  107fe8:	31 c0                	xor    %eax,%eax
  107fea:	83 c4 0c             	add    $0xc,%esp
  107fed:	c3                   	ret
  107fee:	66 90                	xchg   %ax,%ax

00107ff0 <sys_sleep>:
  107ff0:	83 ec 18             	sub    $0x18,%esp
  107ff3:	ff 74 24 1c          	push   0x1c(%esp)
  107ff7:	e8 b4 e2 ff ff       	call   1062b0 <task_sleep>
  107ffc:	31 c0                	xor    %eax,%eax
  107ffe:	83 c4 1c             	add    $0x1c,%esp
  108001:	c3                   	ret
  108002:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  108009:	00 
  10800a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00108010 <sys_kill>:
  108010:	83 ec 18             	sub    $0x18,%esp
  108013:	ff 74 24 1c          	push   0x1c(%esp)
  108017:	e8 04 c0 ff ff       	call   104020 <proc_find>
  10801c:	83 c4 10             	add    $0x10,%esp
  10801f:	85 c0                	test   %eax,%eax
  108021:	74 16                	je     108039 <sys_kill+0x29>
  108023:	83 ec 08             	sub    $0x8,%esp
  108026:	ff 74 24 1c          	push   0x1c(%esp)
  10802a:	50                   	push   %eax
  10802b:	e8 90 bc ff ff       	call   103cc0 <proc_signal>
  108030:	83 c4 10             	add    $0x10,%esp
  108033:	31 c0                	xor    %eax,%eax
  108035:	83 c4 0c             	add    $0xc,%esp
  108038:	c3                   	ret
  108039:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  10803e:	eb f5                	jmp    108035 <sys_kill+0x25>

00108040 <sys_getpid>:
  108040:	83 ec 0c             	sub    $0xc,%esp
  108043:	e8 18 c0 ff ff       	call   104060 <proc_current>
  108048:	ba 01 00 00 00       	mov    $0x1,%edx
  10804d:	85 c0                	test   %eax,%eax
  10804f:	74 02                	je     108053 <sys_getpid+0x13>
  108051:	8b 10                	mov    (%eax),%edx
  108053:	89 d0                	mov    %edx,%eax
  108055:	83 c4 0c             	add    $0xc,%esp
  108058:	c3                   	ret
  108059:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00108060 <sys_write>:
  108060:	57                   	push   %edi
  108061:	56                   	push   %esi
  108062:	53                   	push   %ebx
  108063:	8b 44 24 10          	mov    0x10(%esp),%eax
  108067:	8b 5c 24 14          	mov    0x14(%esp),%ebx
  10806b:	8b 7c 24 18          	mov    0x18(%esp),%edi
  10806f:	83 e8 01             	sub    $0x1,%eax
  108072:	83 f8 01             	cmp    $0x1,%eax
  108075:	77 25                	ja     10809c <sys_write+0x3c>
  108077:	85 ff                	test   %edi,%edi
  108079:	74 1b                	je     108096 <sys_write+0x36>
  10807b:	8d 34 3b             	lea    (%ebx,%edi,1),%esi
  10807e:	66 90                	xchg   %ax,%ax
  108080:	0f be 03             	movsbl (%ebx),%eax
  108083:	83 ec 0c             	sub    $0xc,%esp
  108086:	83 c3 01             	add    $0x1,%ebx
  108089:	50                   	push   %eax
  10808a:	e8 31 c9 ff ff       	call   1049c0 <serial_putc>
  10808f:	83 c4 10             	add    $0x10,%esp
  108092:	39 f3                	cmp    %esi,%ebx
  108094:	75 ea                	jne    108080 <sys_write+0x20>
  108096:	89 f8                	mov    %edi,%eax
  108098:	5b                   	pop    %ebx
  108099:	5e                   	pop    %esi
  10809a:	5f                   	pop    %edi
  10809b:	c3                   	ret
  10809c:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  1080a1:	eb f5                	jmp    108098 <sys_write+0x38>
  1080a3:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1080aa:	00 
  1080ab:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

001080b0 <sys_read>:
  1080b0:	57                   	push   %edi
  1080b1:	56                   	push   %esi
  1080b2:	53                   	push   %ebx
  1080b3:	8b 7c 24 10          	mov    0x10(%esp),%edi
  1080b7:	8b 74 24 14          	mov    0x14(%esp),%esi
  1080bb:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  1080bf:	85 ff                	test   %edi,%edi
  1080c1:	75 33                	jne    1080f6 <sys_read+0x46>
  1080c3:	85 db                	test   %ebx,%ebx
  1080c5:	75 0d                	jne    1080d4 <sys_read+0x24>
  1080c7:	eb 27                	jmp    1080f0 <sys_read+0x40>
  1080c9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  1080d0:	39 df                	cmp    %ebx,%edi
  1080d2:	74 1c                	je     1080f0 <sys_read+0x40>
  1080d4:	e8 17 ca ff ff       	call   104af0 <serial_getc>
  1080d9:	88 04 3e             	mov    %al,(%esi,%edi,1)
  1080dc:	83 c7 01             	add    $0x1,%edi
  1080df:	3c 0a                	cmp    $0xa,%al
  1080e1:	75 ed                	jne    1080d0 <sys_read+0x20>
  1080e3:	89 f8                	mov    %edi,%eax
  1080e5:	5b                   	pop    %ebx
  1080e6:	5e                   	pop    %esi
  1080e7:	5f                   	pop    %edi
  1080e8:	c3                   	ret
  1080e9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  1080f0:	89 d8                	mov    %ebx,%eax
  1080f2:	5b                   	pop    %ebx
  1080f3:	5e                   	pop    %esi
  1080f4:	5f                   	pop    %edi
  1080f5:	c3                   	ret
  1080f6:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  1080fb:	eb f5                	jmp    1080f2 <sys_read+0x42>
  1080fd:	8d 76 00             	lea    0x0(%esi),%esi

00108100 <sys_exit>:
  108100:	83 ec 18             	sub    $0x18,%esp
  108103:	ff 74 24 1c          	push   0x1c(%esp)
  108107:	e8 64 bf ff ff       	call   104070 <proc_exit>
  10810c:	31 c0                	xor    %eax,%eax
  10810e:	83 c4 1c             	add    $0x1c,%esp
  108111:	c3                   	ret
  108112:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  108119:	00 
  10811a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00108120 <syscall_init>:
  108120:	83 ec 10             	sub    $0x10,%esp
  108123:	68 00 04 00 00       	push   $0x400
  108128:	6a 00                	push   $0x0
  10812a:	68 40 33 12 00       	push   $0x123340
  10812f:	e8 2c 94 ff ff       	call   101560 <memset>
  108134:	58                   	pop    %eax
  108135:	5a                   	pop    %edx
  108136:	68 80 7f 10 00       	push   $0x107f80
  10813b:	68 80 00 00 00       	push   $0x80
  108140:	c7 05 44 33 12 00 00 	movl   $0x108100,0x123344
  108147:	81 10 00 
  10814a:	c7 05 4c 33 12 00 b0 	movl   $0x1080b0,0x12334c
  108151:	80 10 00 
  108154:	c7 05 50 33 12 00 60 	movl   $0x108060,0x123350
  10815b:	80 10 00 
  10815e:	c7 05 90 33 12 00 40 	movl   $0x108040,0x123390
  108165:	80 10 00 
  108168:	c7 05 d4 33 12 00 10 	movl   $0x108010,0x1233d4
  10816f:	80 10 00 
  108172:	c7 05 c8 35 12 00 f0 	movl   $0x107ff0,0x1235c8
  108179:	7f 10 00 
  10817c:	c7 05 b8 35 12 00 e0 	movl   $0x107fe0,0x1235b8
  108183:	7f 10 00 
  108186:	e8 85 a6 ff ff       	call   102810 <exception_register>
  10818b:	83 c4 0c             	add    $0xc,%esp
  10818e:	68 9f 00 00 00       	push   $0x9f
  108193:	68 9c 9f 10 00       	push   $0x109f9c
  108198:	6a 01                	push   $0x1
  10819a:	e8 91 c9 ff ff       	call   104b30 <klog>
  10819f:	83 c4 1c             	add    $0x1c,%esp
  1081a2:	c3                   	ret
  1081a3:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1081aa:	00 
  1081ab:	2e 8d 74 26 00       	lea    %cs:0x0(%esi,%eiz,1),%esi

001081b0 <elf32_load>:
  1081b0:	55                   	push   %ebp
  1081b1:	57                   	push   %edi
  1081b2:	56                   	push   %esi
  1081b3:	53                   	push   %ebx
  1081b4:	83 ec 1c             	sub    $0x1c,%esp
  1081b7:	8b 6c 24 30          	mov    0x30(%esp),%ebp
  1081bb:	85 ed                	test   %ebp,%ebp
  1081bd:	0f 84 b2 01 00 00    	je     108375 <elf32_load+0x1c5>
  1081c3:	83 7c 24 34 33       	cmpl   $0x33,0x34(%esp)
  1081c8:	0f 86 a7 01 00 00    	jbe    108375 <elf32_load+0x1c5>
  1081ce:	80 7d 00 7f          	cmpb   $0x7f,0x0(%ebp)
  1081d2:	0f 85 81 01 00 00    	jne    108359 <elf32_load+0x1a9>
  1081d8:	80 7d 01 45          	cmpb   $0x45,0x1(%ebp)
  1081dc:	0f 85 77 01 00 00    	jne    108359 <elf32_load+0x1a9>
  1081e2:	80 7d 02 4c          	cmpb   $0x4c,0x2(%ebp)
  1081e6:	0f 85 6d 01 00 00    	jne    108359 <elf32_load+0x1a9>
  1081ec:	80 7d 03 46          	cmpb   $0x46,0x3(%ebp)
  1081f0:	0f 85 63 01 00 00    	jne    108359 <elf32_load+0x1a9>
  1081f6:	66 83 7d 10 02       	cmpw   $0x2,0x10(%ebp)
  1081fb:	0f 85 9a 01 00 00    	jne    10839b <elf32_load+0x1eb>
  108201:	66 83 7d 12 03       	cmpw   $0x3,0x12(%ebp)
  108206:	0f 85 73 01 00 00    	jne    10837f <elf32_load+0x1cf>
  10820c:	8b 7d 1c             	mov    0x1c(%ebp),%edi
  10820f:	31 d2                	xor    %edx,%edx
  108211:	01 ef                	add    %ebp,%edi
  108213:	66 83 7d 2c 00       	cmpw   $0x0,0x2c(%ebp)
  108218:	0f 84 f4 00 00 00    	je     108312 <elf32_load+0x162>
  10821e:	89 6c 24 30          	mov    %ebp,0x30(%esp)
  108222:	89 d6                	mov    %edx,%esi
  108224:	eb 1e                	jmp    108244 <elf32_load+0x94>
  108226:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10822d:	00 
  10822e:	66 90                	xchg   %ax,%ax
  108230:	8b 44 24 30          	mov    0x30(%esp),%eax
  108234:	83 c6 01             	add    $0x1,%esi
  108237:	83 c7 20             	add    $0x20,%edi
  10823a:	66 3b 70 2c          	cmp    0x2c(%eax),%si
  10823e:	0f 83 cc 00 00 00    	jae    108310 <elf32_load+0x160>
  108244:	83 3f 01             	cmpl   $0x1,(%edi)
  108247:	75 e7                	jne    108230 <elf32_load+0x80>
  108249:	8b 4f 08             	mov    0x8(%edi),%ecx
  10824c:	8b 47 14             	mov    0x14(%edi),%eax
  10824f:	89 cd                	mov    %ecx,%ebp
  108251:	8d 9c 01 ff 0f 00 00 	lea    0xfff(%ecx,%eax,1),%ebx
  108258:	81 e5 00 f0 ff ff    	and    $0xfffff000,%ebp
  10825e:	81 e3 00 f0 ff ff    	and    $0xfffff000,%ebx
  108264:	29 eb                	sub    %ebp,%ebx
  108266:	c1 eb 0c             	shr    $0xc,%ebx
  108269:	74 4b                	je     1082b6 <elf32_load+0x106>
  10826b:	c1 e3 0c             	shl    $0xc,%ebx
  10826e:	01 eb                	add    %ebp,%ebx
  108270:	eb 1f                	jmp    108291 <elf32_load+0xe1>
  108272:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  108278:	83 ec 04             	sub    $0x4,%esp
  10827b:	6a 07                	push   $0x7
  10827d:	50                   	push   %eax
  10827e:	55                   	push   %ebp
  10827f:	81 c5 00 10 00 00    	add    $0x1000,%ebp
  108285:	e8 b6 ae ff ff       	call   103140 <paging_map>
  10828a:	83 c4 10             	add    $0x10,%esp
  10828d:	39 eb                	cmp    %ebp,%ebx
  10828f:	74 1f                	je     1082b0 <elf32_load+0x100>
  108291:	e8 aa ab ff ff       	call   102e40 <pmm_alloc_page>
  108296:	85 c0                	test   %eax,%eax
  108298:	75 de                	jne    108278 <elf32_load+0xc8>
  10829a:	b8 fb ff ff ff       	mov    $0xfffffffb,%eax
  10829f:	83 c4 1c             	add    $0x1c,%esp
  1082a2:	5b                   	pop    %ebx
  1082a3:	5e                   	pop    %esi
  1082a4:	5f                   	pop    %edi
  1082a5:	5d                   	pop    %ebp
  1082a6:	c3                   	ret
  1082a7:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1082ae:	00 
  1082af:	90                   	nop
  1082b0:	8b 4f 08             	mov    0x8(%edi),%ecx
  1082b3:	8b 47 14             	mov    0x14(%edi),%eax
  1082b6:	83 ec 04             	sub    $0x4,%esp
  1082b9:	50                   	push   %eax
  1082ba:	6a 00                	push   $0x0
  1082bc:	51                   	push   %ecx
  1082bd:	89 4c 24 1c          	mov    %ecx,0x1c(%esp)
  1082c1:	e8 9a 92 ff ff       	call   101560 <memset>
  1082c6:	8b 47 10             	mov    0x10(%edi),%eax
  1082c9:	83 c4 10             	add    $0x10,%esp
  1082cc:	85 c0                	test   %eax,%eax
  1082ce:	74 10                	je     1082e0 <elf32_load+0x130>
  1082d0:	8b 57 04             	mov    0x4(%edi),%edx
  1082d3:	8b 4c 24 0c          	mov    0xc(%esp),%ecx
  1082d7:	8d 1c 10             	lea    (%eax,%edx,1),%ebx
  1082da:	39 5c 24 34          	cmp    %ebx,0x34(%esp)
  1082de:	73 62                	jae    108342 <elf32_load+0x192>
  1082e0:	50                   	push   %eax
  1082e1:	83 c6 01             	add    $0x1,%esi
  1082e4:	83 c7 20             	add    $0x20,%edi
  1082e7:	ff 77 e8             	push   -0x18(%edi)
  1082ea:	68 f4 9f 10 00       	push   $0x109ff4
  1082ef:	6a 01                	push   $0x1
  1082f1:	e8 3a c8 ff ff       	call   104b30 <klog>
  1082f6:	8b 44 24 40          	mov    0x40(%esp),%eax
  1082fa:	83 c4 10             	add    $0x10,%esp
  1082fd:	66 3b 70 2c          	cmp    0x2c(%eax),%si
  108301:	0f 82 3d ff ff ff    	jb     108244 <elf32_load+0x94>
  108307:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  10830e:	00 
  10830f:	90                   	nop
  108310:	89 c5                	mov    %eax,%ebp
  108312:	8b 44 24 38          	mov    0x38(%esp),%eax
  108316:	85 c0                	test   %eax,%eax
  108318:	74 09                	je     108323 <elf32_load+0x173>
  10831a:	8b 45 18             	mov    0x18(%ebp),%eax
  10831d:	8b 54 24 38          	mov    0x38(%esp),%edx
  108321:	89 02                	mov    %eax,(%edx)
  108323:	83 ec 04             	sub    $0x4,%esp
  108326:	ff 75 18             	push   0x18(%ebp)
  108329:	68 18 a0 10 00       	push   $0x10a018
  10832e:	6a 01                	push   $0x1
  108330:	e8 fb c7 ff ff       	call   104b30 <klog>
  108335:	83 c4 10             	add    $0x10,%esp
  108338:	31 c0                	xor    %eax,%eax
  10833a:	83 c4 1c             	add    $0x1c,%esp
  10833d:	5b                   	pop    %ebx
  10833e:	5e                   	pop    %esi
  10833f:	5f                   	pop    %edi
  108340:	5d                   	pop    %ebp
  108341:	c3                   	ret
  108342:	83 ec 04             	sub    $0x4,%esp
  108345:	50                   	push   %eax
  108346:	03 54 24 38          	add    0x38(%esp),%edx
  10834a:	52                   	push   %edx
  10834b:	51                   	push   %ecx
  10834c:	e8 5f 92 ff ff       	call   1015b0 <memcpy>
  108351:	8b 47 10             	mov    0x10(%edi),%eax
  108354:	83 c4 10             	add    $0x10,%esp
  108357:	eb 87                	jmp    1082e0 <elf32_load+0x130>
  108359:	83 ec 08             	sub    $0x8,%esp
  10835c:	68 97 95 10 00       	push   $0x109597
  108361:	6a 03                	push   $0x3
  108363:	e8 c8 c7 ff ff       	call   104b30 <klog>
  108368:	83 c4 10             	add    $0x10,%esp
  10836b:	b8 fe ff ff ff       	mov    $0xfffffffe,%eax
  108370:	e9 2a ff ff ff       	jmp    10829f <elf32_load+0xef>
  108375:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  10837a:	e9 20 ff ff ff       	jmp    10829f <elf32_load+0xef>
  10837f:	83 ec 08             	sub    $0x8,%esp
  108382:	68 ae 95 10 00       	push   $0x1095ae
  108387:	6a 03                	push   $0x3
  108389:	e8 a2 c7 ff ff       	call   104b30 <klog>
  10838e:	83 c4 10             	add    $0x10,%esp
  108391:	b8 fc ff ff ff       	mov    $0xfffffffc,%eax
  108396:	e9 04 ff ff ff       	jmp    10829f <elf32_load+0xef>
  10839b:	83 ec 08             	sub    $0x8,%esp
  10839e:	68 d4 9f 10 00       	push   $0x109fd4
  1083a3:	6a 03                	push   $0x3
  1083a5:	e8 86 c7 ff ff       	call   104b30 <klog>
  1083aa:	83 c4 10             	add    $0x10,%esp
  1083ad:	b8 fd ff ff ff       	mov    $0xfffffffd,%eax
  1083b2:	e9 e8 fe ff ff       	jmp    10829f <elf32_load+0xef>
  1083b7:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1083be:	00 
  1083bf:	90                   	nop

001083c0 <user_mode_enter>:
  1083c0:	56                   	push   %esi
  1083c1:	5e                   	pop    %esi
  1083c2:	83 ec 0c             	sub    $0xc,%esp
  1083c5:	e8 46 df ff ff       	call   106310 <sched_current>
  1083ca:	85 c0                	test   %eax,%eax
  1083cc:	74 0e                	je     1083dc <user_mode_enter+0x1c>
  1083ce:	83 ec 0c             	sub    $0xc,%esp
  1083d1:	ff 70 30             	push   0x30(%eax)
  1083d4:	e8 27 9d ff ff       	call   102100 <tss_set_kernel_stack>
  1083d9:	83 c4 10             	add    $0x10,%esp
  1083dc:	66 b8 23 00          	mov    $0x23,%ax
  1083e0:	8e d8                	mov    %eax,%ds
  1083e2:	8e c0                	mov    %eax,%es
  1083e4:	8e e0                	mov    %eax,%fs
  1083e6:	8e e8                	mov    %eax,%gs
  1083e8:	8b 54 24 10          	mov    0x10(%esp),%edx
  1083ec:	8b 44 24 14          	mov    0x14(%esp),%eax
  1083f0:	6a 23                	push   $0x23
  1083f2:	50                   	push   %eax
  1083f3:	9c                   	pushf
  1083f4:	81 0c 24 00 02 00 00 	orl    $0x200,(%esp)
  1083fb:	6a 1b                	push   $0x1b
  1083fd:	52                   	push   %edx
  1083fe:	cf                   	iret
  1083ff:	90                   	nop
  108400:	f4                   	hlt
  108401:	eb fd                	jmp    108400 <user_mode_enter+0x40>
  108403:	66 90                	xchg   %ax,%ax
  108405:	66 90                	xchg   %ax,%ax
  108407:	66 90                	xchg   %ax,%ax
  108409:	66 90                	xchg   %ax,%ax
  10840b:	66 90                	xchg   %ax,%ax
  10840d:	66 90                	xchg   %ax,%ax
  10840f:	90                   	nop

00108410 <debug_shell_run>:
  108410:	55                   	push   %ebp
  108411:	57                   	push   %edi
  108412:	bf d3 4d 62 10       	mov    $0x10624dd3,%edi
  108417:	56                   	push   %esi
  108418:	53                   	push   %ebx
  108419:	81 ec 68 01 00 00    	sub    $0x168,%esp
  10841f:	68 be 95 10 00       	push   $0x1095be
  108424:	e8 c7 c5 ff ff       	call   1049f0 <serial_puts>
  108429:	c7 04 24 3c a0 10 00 	movl   $0x10a03c,(%esp)
  108430:	e8 bb c5 ff ff       	call   1049f0 <serial_puts>
  108435:	c7 04 24 b8 a0 10 00 	movl   $0x10a0b8,(%esp)
  10843c:	e8 af c5 ff ff       	call   1049f0 <serial_puts>
  108441:	c7 04 24 e8 a0 10 00 	movl   $0x10a0e8,(%esp)
  108448:	e8 a3 c5 ff ff       	call   1049f0 <serial_puts>
  10844d:	c7 04 24 18 a1 10 00 	movl   $0x10a118,(%esp)
  108454:	e8 97 c5 ff ff       	call   1049f0 <serial_puts>
  108459:	83 c4 10             	add    $0x10,%esp
  10845c:	8d 6c 24 10          	lea    0x10(%esp),%ebp
  108460:	e8 bb de ff ff       	call   106320 <sched_uptime_ms>
  108465:	83 ec 08             	sub    $0x8,%esp
  108468:	31 db                	xor    %ebx,%ebx
  10846a:	f7 e7                	mul    %edi
  10846c:	c1 ea 06             	shr    $0x6,%edx
  10846f:	52                   	push   %edx
  108470:	68 c1 95 10 00       	push   $0x1095c1
  108475:	e8 16 c6 ff ff       	call   104a90 <serial_printf>
  10847a:	c6 44 24 60 00       	movb   $0x0,0x60(%esp)
  10847f:	83 c4 10             	add    $0x10,%esp
  108482:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  108488:	e8 63 c6 ff ff       	call   104af0 <serial_getc>
  10848d:	3c 0d                	cmp    $0xd,%al
  10848f:	74 4f                	je     1084e0 <debug_shell_run+0xd0>
  108491:	3c 0a                	cmp    $0xa,%al
  108493:	74 4b                	je     1084e0 <debug_shell_run+0xd0>
  108495:	3c 08                	cmp    $0x8,%al
  108497:	0f 84 33 01 00 00    	je     1085d0 <debug_shell_run+0x1c0>
  10849d:	3c 7f                	cmp    $0x7f,%al
  10849f:	0f 84 2b 01 00 00    	je     1085d0 <debug_shell_run+0x1c0>
  1084a5:	8d 50 e0             	lea    -0x20(%eax),%edx
  1084a8:	80 fa 5e             	cmp    $0x5e,%dl
  1084ab:	77 db                	ja     108488 <debug_shell_run+0x78>
  1084ad:	8d 73 01             	lea    0x1(%ebx),%esi
  1084b0:	81 fe ff 00 00 00    	cmp    $0xff,%esi
  1084b6:	77 d0                	ja     108488 <debug_shell_run+0x78>
  1084b8:	88 44 1c 50          	mov    %al,0x50(%esp,%ebx,1)
  1084bc:	83 ec 0c             	sub    $0xc,%esp
  1084bf:	0f be c0             	movsbl %al,%eax
  1084c2:	89 f3                	mov    %esi,%ebx
  1084c4:	50                   	push   %eax
  1084c5:	e8 f6 c4 ff ff       	call   1049c0 <serial_putc>
  1084ca:	83 c4 10             	add    $0x10,%esp
  1084cd:	e8 1e c6 ff ff       	call   104af0 <serial_getc>
  1084d2:	3c 0d                	cmp    $0xd,%al
  1084d4:	75 bb                	jne    108491 <debug_shell_run+0x81>
  1084d6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1084dd:	00 
  1084de:	66 90                	xchg   %ax,%ax
  1084e0:	c6 44 1c 50 00       	movb   $0x0,0x50(%esp,%ebx,1)
  1084e5:	83 ec 0c             	sub    $0xc,%esp
  1084e8:	68 bf 95 10 00       	push   $0x1095bf
  1084ed:	e8 fe c4 ff ff       	call   1049f0 <serial_puts>
  1084f2:	0f b6 44 24 60       	movzbl 0x60(%esp),%eax
  1084f7:	83 c4 10             	add    $0x10,%esp
  1084fa:	8d 74 24 50          	lea    0x50(%esp),%esi
  1084fe:	3c 20                	cmp    $0x20,%al
  108500:	75 11                	jne    108513 <debug_shell_run+0x103>
  108502:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  108508:	0f b6 46 01          	movzbl 0x1(%esi),%eax
  10850c:	83 c6 01             	add    $0x1,%esi
  10850f:	3c 20                	cmp    $0x20,%al
  108511:	74 f5                	je     108508 <debug_shell_run+0xf8>
  108513:	84 c0                	test   %al,%al
  108515:	0f 84 45 ff ff ff    	je     108460 <debug_shell_run+0x50>
  10851b:	a1 40 37 12 00       	mov    0x123740,%eax
  108520:	83 ec 04             	sub    $0x4,%esp
  108523:	68 ff 00 00 00       	push   $0xff
  108528:	99                   	cltd
  108529:	56                   	push   %esi
  10852a:	c1 ea 1d             	shr    $0x1d,%edx
  10852d:	01 d0                	add    %edx,%eax
  10852f:	83 e0 07             	and    $0x7,%eax
  108532:	29 d0                	sub    %edx,%eax
  108534:	c1 e0 08             	shl    $0x8,%eax
  108537:	05 60 37 12 00       	add    $0x123760,%eax
  10853c:	50                   	push   %eax
  10853d:	e8 ae 91 ff ff       	call   1016f0 <strncpy>
  108542:	a1 44 37 12 00       	mov    0x123744,%eax
  108547:	83 05 40 37 12 00 01 	addl   $0x1,0x123740
  10854e:	83 c4 10             	add    $0x10,%esp
  108551:	83 f8 07             	cmp    $0x7,%eax
  108554:	7f 08                	jg     10855e <debug_shell_run+0x14e>
  108556:	83 c0 01             	add    $0x1,%eax
  108559:	a3 44 37 12 00       	mov    %eax,0x123744
  10855e:	0f b6 16             	movzbl (%esi),%edx
  108561:	31 c0                	xor    %eax,%eax
  108563:	f6 c2 df             	test   $0xdf,%dl
  108566:	75 0d                	jne    108575 <debug_shell_run+0x165>
  108568:	eb 1d                	jmp    108587 <debug_shell_run+0x177>
  10856a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  108570:	83 f8 3f             	cmp    $0x3f,%eax
  108573:	74 12                	je     108587 <debug_shell_run+0x177>
  108575:	83 c6 01             	add    $0x1,%esi
  108578:	83 c0 01             	add    $0x1,%eax
  10857b:	88 54 04 0f          	mov    %dl,0xf(%esp,%eax,1)
  10857f:	0f b6 16             	movzbl (%esi),%edx
  108582:	f6 c2 df             	test   $0xdf,%dl
  108585:	75 e9                	jne    108570 <debug_shell_run+0x160>
  108587:	c6 44 04 10 00       	movb   $0x0,0x10(%esp,%eax,1)
  10858c:	80 fa 20             	cmp    $0x20,%dl
  10858f:	75 0f                	jne    1085a0 <debug_shell_run+0x190>
  108591:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  108598:	83 c6 01             	add    $0x1,%esi
  10859b:	80 3e 20             	cmpb   $0x20,(%esi)
  10859e:	74 f8                	je     108598 <debug_shell_run+0x188>
  1085a0:	83 ec 08             	sub    $0x8,%esp
  1085a3:	68 da 95 10 00       	push   $0x1095da
  1085a8:	55                   	push   %ebp
  1085a9:	e8 c2 91 ff ff       	call   101770 <strcmp>
  1085ae:	83 c4 10             	add    $0x10,%esp
  1085b1:	85 c0                	test   %eax,%eax
  1085b3:	75 3b                	jne    1085f0 <debug_shell_run+0x1e0>
  1085b5:	83 ec 0c             	sub    $0xc,%esp
  1085b8:	68 94 a1 10 00       	push   $0x10a194
  1085bd:	e8 2e c4 ff ff       	call   1049f0 <serial_puts>
  1085c2:	83 c4 10             	add    $0x10,%esp
  1085c5:	e9 96 fe ff ff       	jmp    108460 <debug_shell_run+0x50>
  1085ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  1085d0:	85 db                	test   %ebx,%ebx
  1085d2:	0f 84 b0 fe ff ff    	je     108488 <debug_shell_run+0x78>
  1085d8:	83 ec 0c             	sub    $0xc,%esp
  1085db:	83 eb 01             	sub    $0x1,%ebx
  1085de:	68 d6 95 10 00       	push   $0x1095d6
  1085e3:	e8 08 c4 ff ff       	call   1049f0 <serial_puts>
  1085e8:	83 c4 10             	add    $0x10,%esp
  1085eb:	e9 98 fe ff ff       	jmp    108488 <debug_shell_run+0x78>
  1085f0:	83 ec 08             	sub    $0x8,%esp
  1085f3:	68 df 95 10 00       	push   $0x1095df
  1085f8:	55                   	push   %ebp
  1085f9:	e8 72 91 ff ff       	call   101770 <strcmp>
  1085fe:	83 c4 10             	add    $0x10,%esp
  108601:	85 c0                	test   %eax,%eax
  108603:	75 5b                	jne    108660 <debug_shell_run+0x250>
  108605:	e8 b6 aa ff ff       	call   1030c0 <pmm_free_page_count>
  10860a:	89 c3                	mov    %eax,%ebx
  10860c:	e8 bf aa ff ff       	call   1030d0 <pmm_total_page_count>
  108611:	83 ec 0c             	sub    $0xc,%esp
  108614:	89 c2                	mov    %eax,%edx
  108616:	c1 e2 0c             	shl    $0xc,%edx
  108619:	c1 ea 14             	shr    $0x14,%edx
  10861c:	52                   	push   %edx
  10861d:	89 da                	mov    %ebx,%edx
  10861f:	c1 e2 0c             	shl    $0xc,%edx
  108622:	c1 ea 14             	shr    $0x14,%edx
  108625:	52                   	push   %edx
  108626:	50                   	push   %eax
  108627:	53                   	push   %ebx
  108628:	68 d8 a3 10 00       	push   $0x10a3d8
  10862d:	e8 5e c4 ff ff       	call   104a90 <serial_printf>
  108632:	83 c4 20             	add    $0x20,%esp
  108635:	e8 46 b0 ff ff       	call   103680 <heap_available>
  10863a:	89 c3                	mov    %eax,%ebx
  10863c:	e8 2f b0 ff ff       	call   103670 <heap_used>
  108641:	c1 eb 0a             	shr    $0xa,%ebx
  108644:	83 ec 04             	sub    $0x4,%esp
  108647:	c1 e8 0a             	shr    $0xa,%eax
  10864a:	53                   	push   %ebx
  10864b:	50                   	push   %eax
  10864c:	68 fc a3 10 00       	push   $0x10a3fc
  108651:	e8 3a c4 ff ff       	call   104a90 <serial_printf>
  108656:	83 c4 10             	add    $0x10,%esp
  108659:	e9 02 fe ff ff       	jmp    108460 <debug_shell_run+0x50>
  10865e:	66 90                	xchg   %ax,%ax
  108660:	83 ec 08             	sub    $0x8,%esp
  108663:	68 e3 95 10 00       	push   $0x1095e3
  108668:	55                   	push   %ebp
  108669:	e8 02 91 ff ff       	call   101770 <strcmp>
  10866e:	83 c4 10             	add    $0x10,%esp
  108671:	85 c0                	test   %eax,%eax
  108673:	75 0a                	jne    10867f <debug_shell_run+0x26f>
  108675:	e8 e6 dc ff ff       	call   106360 <sched_dump>
  10867a:	e9 e1 fd ff ff       	jmp    108460 <debug_shell_run+0x50>
  10867f:	83 ec 08             	sub    $0x8,%esp
  108682:	68 e6 95 10 00       	push   $0x1095e6
  108687:	55                   	push   %ebp
  108688:	e8 e3 90 ff ff       	call   101770 <strcmp>
  10868d:	83 c4 10             	add    $0x10,%esp
  108690:	85 c0                	test   %eax,%eax
  108692:	75 0a                	jne    10869e <debug_shell_run+0x28e>
  108694:	e8 27 c9 ff ff       	call   104fc0 <pci_dump>
  108699:	e9 c2 fd ff ff       	jmp    108460 <debug_shell_run+0x50>
  10869e:	83 ec 08             	sub    $0x8,%esp
  1086a1:	68 ea 95 10 00       	push   $0x1095ea
  1086a6:	55                   	push   %ebp
  1086a7:	e8 c4 90 ff ff       	call   101770 <strcmp>
  1086ac:	83 c4 10             	add    $0x10,%esp
  1086af:	85 c0                	test   %eax,%eax
  1086b1:	75 0a                	jne    1086bd <debug_shell_run+0x2ad>
  1086b3:	e8 08 e9 ff ff       	call   106fc0 <vfs_dump>
  1086b8:	e9 a3 fd ff ff       	jmp    108460 <debug_shell_run+0x50>
  1086bd:	83 ec 08             	sub    $0x8,%esp
  1086c0:	68 ee 95 10 00       	push   $0x1095ee
  1086c5:	55                   	push   %ebp
  1086c6:	e8 a5 90 ff ff       	call   101770 <strcmp>
  1086cb:	83 c4 10             	add    $0x10,%esp
  1086ce:	85 c0                	test   %eax,%eax
  1086d0:	75 0a                	jne    1086dc <debug_shell_run+0x2cc>
  1086d2:	e8 b9 f7 ff ff       	call   107e90 <net_dump>
  1086d7:	e9 84 fd ff ff       	jmp    108460 <debug_shell_run+0x50>
  1086dc:	83 ec 08             	sub    $0x8,%esp
  1086df:	68 f2 95 10 00       	push   $0x1095f2
  1086e4:	55                   	push   %ebp
  1086e5:	e8 86 90 ff ff       	call   101770 <strcmp>
  1086ea:	83 c4 10             	add    $0x10,%esp
  1086ed:	85 c0                	test   %eax,%eax
  1086ef:	75 25                	jne    108716 <debug_shell_run+0x306>
  1086f1:	e8 2a dc ff ff       	call   106320 <sched_uptime_ms>
  1086f6:	bb e8 03 00 00       	mov    $0x3e8,%ebx
  1086fb:	31 d2                	xor    %edx,%edx
  1086fd:	56                   	push   %esi
  1086fe:	89 c1                	mov    %eax,%ecx
  108700:	f7 f3                	div    %ebx
  108702:	50                   	push   %eax
  108703:	51                   	push   %ecx
  108704:	68 f9 95 10 00       	push   $0x1095f9
  108709:	e8 82 c3 ff ff       	call   104a90 <serial_printf>
  10870e:	83 c4 10             	add    $0x10,%esp
  108711:	e9 4a fd ff ff       	jmp    108460 <debug_shell_run+0x50>
  108716:	53                   	push   %ebx
  108717:	53                   	push   %ebx
  108718:	68 15 96 10 00       	push   $0x109615
  10871d:	55                   	push   %ebp
  10871e:	e8 4d 90 ff ff       	call   101770 <strcmp>
  108723:	83 c4 10             	add    $0x10,%esp
  108726:	85 c0                	test   %eax,%eax
  108728:	74 3e                	je     108768 <debug_shell_run+0x358>
  10872a:	51                   	push   %ecx
  10872b:	51                   	push   %ecx
  10872c:	68 1b 96 10 00       	push   $0x10961b
  108731:	55                   	push   %ebp
  108732:	e8 39 90 ff ff       	call   101770 <strcmp>
  108737:	83 c4 10             	add    $0x10,%esp
  10873a:	85 c0                	test   %eax,%eax
  10873c:	75 34                	jne    108772 <debug_shell_run+0x362>
  10873e:	83 ec 0c             	sub    $0xc,%esp
  108741:	56                   	push   %esi
  108742:	e8 a9 c2 ff ff       	call   1049f0 <serial_puts>
  108747:	c7 04 24 0a 00 00 00 	movl   $0xa,(%esp)
  10874e:	e8 6d c2 ff ff       	call   1049c0 <serial_putc>
  108753:	58                   	pop    %eax
  108754:	5a                   	pop    %edx
  108755:	56                   	push   %esi
  108756:	68 93 90 10 00       	push   $0x109093
  10875b:	e8 e0 be ff ff       	call   104640 <vga_printf>
  108760:	83 c4 10             	add    $0x10,%esp
  108763:	e9 f8 fc ff ff       	jmp    108460 <debug_shell_run+0x50>
  108768:	e8 b3 bb ff ff       	call   104320 <vga_clear>
  10876d:	e9 ee fc ff ff       	jmp    108460 <debug_shell_run+0x50>
  108772:	50                   	push   %eax
  108773:	50                   	push   %eax
  108774:	68 20 96 10 00       	push   $0x109620
  108779:	55                   	push   %ebp
  10877a:	e8 f1 8f ff ff       	call   101770 <strcmp>
  10877f:	83 c4 10             	add    $0x10,%esp
  108782:	85 c0                	test   %eax,%eax
  108784:	75 61                	jne    1087e7 <debug_shell_run+0x3d7>
  108786:	80 3e 00             	cmpb   $0x0,(%esi)
  108789:	0f 84 09 01 00 00    	je     108898 <debug_shell_run+0x488>
  10878f:	31 c9                	xor    %ecx,%ecx
  108791:	eb 21                	jmp    1087b4 <debug_shell_run+0x3a4>
  108793:	0f be d8             	movsbl %al,%ebx
  108796:	c1 e1 04             	shl    $0x4,%ecx
  108799:	8d 53 a9             	lea    -0x57(%ebx),%edx
  10879c:	3c 39                	cmp    $0x39,%al
  10879e:	7f 03                	jg     1087a3 <debug_shell_run+0x393>
  1087a0:	8d 53 d0             	lea    -0x30(%ebx),%edx
  1087a3:	01 d1                	add    %edx,%ecx
  1087a5:	80 7e 01 20          	cmpb   $0x20,0x1(%esi)
  1087a9:	8d 46 01             	lea    0x1(%esi),%eax
  1087ac:	0f 84 f7 01 00 00    	je     1089a9 <debug_shell_run+0x599>
  1087b2:	89 c6                	mov    %eax,%esi
  1087b4:	0f b6 06             	movzbl (%esi),%eax
  1087b7:	84 c0                	test   %al,%al
  1087b9:	75 d8                	jne    108793 <debug_shell_run+0x383>
  1087bb:	bb 40 00 00 00       	mov    $0x40,%ebx
  1087c0:	eb 0a                	jmp    1087cc <debug_shell_run+0x3bc>
  1087c2:	6b db 0a             	imul   $0xa,%ebx,%ebx
  1087c5:	83 c6 01             	add    $0x1,%esi
  1087c8:	8d 5c 03 d0          	lea    -0x30(%ebx,%eax,1),%ebx
  1087cc:	0f be 06             	movsbl (%esi),%eax
  1087cf:	84 c0                	test   %al,%al
  1087d1:	75 ef                	jne    1087c2 <debug_shell_run+0x3b2>
  1087d3:	89 ce                	mov    %ecx,%esi
  1087d5:	85 db                	test   %ebx,%ebx
  1087d7:	0f 85 c4 00 00 00    	jne    1088a1 <debug_shell_run+0x491>
  1087dd:	bb 40 00 00 00       	mov    $0x40,%ebx
  1087e2:	e9 ba 00 00 00       	jmp    1088a1 <debug_shell_run+0x491>
  1087e7:	50                   	push   %eax
  1087e8:	31 db                	xor    %ebx,%ebx
  1087ea:	50                   	push   %eax
  1087eb:	68 3b 96 10 00       	push   $0x10963b
  1087f0:	55                   	push   %ebp
  1087f1:	e8 7a 8f ff ff       	call   101770 <strcmp>
  1087f6:	83 c4 10             	add    $0x10,%esp
  1087f9:	85 c0                	test   %eax,%eax
  1087fb:	74 17                	je     108814 <debug_shell_run+0x404>
  1087fd:	eb 3c                	jmp    10883b <debug_shell_run+0x42b>
  1087ff:	0f be c8             	movsbl %al,%ecx
  108802:	c1 e3 04             	shl    $0x4,%ebx
  108805:	8d 51 a9             	lea    -0x57(%ecx),%edx
  108808:	3c 39                	cmp    $0x39,%al
  10880a:	7f 03                	jg     10880f <debug_shell_run+0x3ff>
  10880c:	8d 51 d0             	lea    -0x30(%ecx),%edx
  10880f:	01 d3                	add    %edx,%ebx
  108811:	83 c6 01             	add    $0x1,%esi
  108814:	0f b6 06             	movzbl (%esi),%eax
  108817:	84 c0                	test   %al,%al
  108819:	75 e4                	jne    1087ff <debug_shell_run+0x3ef>
  10881b:	83 ec 0c             	sub    $0xc,%esp
  10881e:	53                   	push   %ebx
  10881f:	e8 2c aa ff ff       	call   103250 <paging_get_phys>
  108824:	83 c4 0c             	add    $0xc,%esp
  108827:	50                   	push   %eax
  108828:	53                   	push   %ebx
  108829:	68 40 96 10 00       	push   $0x109640
  10882e:	e8 5d c2 ff ff       	call   104a90 <serial_printf>
  108833:	83 c4 10             	add    $0x10,%esp
  108836:	e9 25 fc ff ff       	jmp    108460 <debug_shell_run+0x50>
  10883b:	53                   	push   %ebx
  10883c:	53                   	push   %ebx
  10883d:	68 5d 96 10 00       	push   $0x10965d
  108842:	55                   	push   %ebp
  108843:	e8 28 8f ff ff       	call   101770 <strcmp>
  108848:	83 c4 10             	add    $0x10,%esp
  10884b:	85 c0                	test   %eax,%eax
  10884d:	0f 84 5e 01 00 00    	je     1089b1 <debug_shell_run+0x5a1>
  108853:	51                   	push   %ecx
  108854:	51                   	push   %ecx
  108855:	68 64 96 10 00       	push   $0x109664
  10885a:	55                   	push   %ebp
  10885b:	e8 10 8f ff ff       	call   101770 <strcmp>
  108860:	83 c4 10             	add    $0x10,%esp
  108863:	85 c0                	test   %eax,%eax
  108865:	0f 84 2c 01 00 00    	je     108997 <debug_shell_run+0x587>
  10886b:	52                   	push   %edx
  10886c:	52                   	push   %edx
  10886d:	68 6d 96 10 00       	push   $0x10966d
  108872:	55                   	push   %ebp
  108873:	e8 f8 8e ff ff       	call   101770 <strcmp>
  108878:	83 c4 10             	add    $0x10,%esp
  10887b:	85 c0                	test   %eax,%eax
  10887d:	0f 84 19 01 00 00    	je     10899c <debug_shell_run+0x58c>
  108883:	50                   	push   %eax
  108884:	50                   	push   %eax
  108885:	55                   	push   %ebp
  108886:	68 58 a4 10 00       	push   $0x10a458
  10888b:	e8 00 c2 ff ff       	call   104a90 <serial_printf>
  108890:	83 c4 10             	add    $0x10,%esp
  108893:	e9 c8 fb ff ff       	jmp    108460 <debug_shell_run+0x50>
  108898:	31 f6                	xor    %esi,%esi
  10889a:	bb 40 00 00 00       	mov    $0x40,%ebx
  10889f:	31 c9                	xor    %ecx,%ecx
  1088a1:	31 c0                	xor    %eax,%eax
  1088a3:	89 4c 24 0c          	mov    %ecx,0xc(%esp)
  1088a7:	89 44 24 04          	mov    %eax,0x4(%esp)
  1088ab:	50                   	push   %eax
  1088ac:	50                   	push   %eax
  1088ad:	8b 4c 24 14          	mov    0x14(%esp),%ecx
  1088b1:	8b 44 24 0c          	mov    0xc(%esp),%eax
  1088b5:	01 c8                	add    %ecx,%eax
  1088b7:	50                   	push   %eax
  1088b8:	68 28 96 10 00       	push   $0x109628
  1088bd:	e8 ce c1 ff ff       	call   104a90 <serial_printf>
  1088c2:	8b 4c 24 14          	mov    0x14(%esp),%ecx
  1088c6:	83 c4 10             	add    $0x10,%esp
  1088c9:	89 c8                	mov    %ecx,%eax
  1088cb:	39 d9                	cmp    %ebx,%ecx
  1088cd:	0f 83 b2 00 00 00    	jae    108985 <debug_shell_run+0x575>
  1088d3:	83 c1 0f             	add    $0xf,%ecx
  1088d6:	89 5c 24 08          	mov    %ebx,0x8(%esp)
  1088da:	89 c3                	mov    %eax,%ebx
  1088dc:	89 0c 24             	mov    %ecx,(%esp)
  1088df:	eb 0b                	jmp    1088ec <debug_shell_run+0x4dc>
  1088e1:	8b 44 24 08          	mov    0x8(%esp),%eax
  1088e5:	83 c3 01             	add    $0x1,%ebx
  1088e8:	39 c3                	cmp    %eax,%ebx
  1088ea:	73 1d                	jae    108909 <debug_shell_run+0x4f9>
  1088ec:	0f b6 14 1e          	movzbl (%esi,%ebx,1),%edx
  1088f0:	83 ec 08             	sub    $0x8,%esp
  1088f3:	52                   	push   %edx
  1088f4:	68 31 96 10 00       	push   $0x109631
  1088f9:	e8 92 c1 ff ff       	call   104a90 <serial_printf>
  1088fe:	8b 44 24 10          	mov    0x10(%esp),%eax
  108902:	83 c4 10             	add    $0x10,%esp
  108905:	39 c3                	cmp    %eax,%ebx
  108907:	75 d8                	jne    1088e1 <debug_shell_run+0x4d1>
  108909:	8b 5c 24 08          	mov    0x8(%esp),%ebx
  10890d:	83 ec 0c             	sub    $0xc,%esp
  108910:	68 37 96 10 00       	push   $0x109637
  108915:	e8 d6 c0 ff ff       	call   1049f0 <serial_puts>
  10891a:	8b 54 24 14          	mov    0x14(%esp),%edx
  10891e:	83 c4 10             	add    $0x10,%esp
  108921:	89 5c 24 08          	mov    %ebx,0x8(%esp)
  108925:	89 d3                	mov    %edx,%ebx
  108927:	eb 0b                	jmp    108934 <debug_shell_run+0x524>
  108929:	8b 44 24 08          	mov    0x8(%esp),%eax
  10892d:	83 c3 01             	add    $0x1,%ebx
  108930:	39 c3                	cmp    %eax,%ebx
  108932:	73 4d                	jae    108981 <debug_shell_run+0x571>
  108934:	0f b6 04 1e          	movzbl (%esi,%ebx,1),%eax
  108938:	8d 48 e0             	lea    -0x20(%eax),%ecx
  10893b:	80 f9 5f             	cmp    $0x5f,%cl
  10893e:	b9 2e 00 00 00       	mov    $0x2e,%ecx
  108943:	0f 43 c1             	cmovae %ecx,%eax
  108946:	83 ec 0c             	sub    $0xc,%esp
  108949:	50                   	push   %eax
  10894a:	e8 71 c0 ff ff       	call   1049c0 <serial_putc>
  10894f:	83 c4 10             	add    $0x10,%esp
  108952:	39 1c 24             	cmp    %ebx,(%esp)
  108955:	75 d2                	jne    108929 <debug_shell_run+0x519>
  108957:	8b 5c 24 08          	mov    0x8(%esp),%ebx
  10895b:	83 ec 0c             	sub    $0xc,%esp
  10895e:	68 bf 95 10 00       	push   $0x1095bf
  108963:	e8 88 c0 ff ff       	call   1049f0 <serial_puts>
  108968:	83 44 24 14 10       	addl   $0x10,0x14(%esp)
  10896d:	8b 44 24 14          	mov    0x14(%esp),%eax
  108971:	83 c4 10             	add    $0x10,%esp
  108974:	39 d8                	cmp    %ebx,%eax
  108976:	0f 82 2f ff ff ff    	jb     1088ab <debug_shell_run+0x49b>
  10897c:	e9 df fa ff ff       	jmp    108460 <debug_shell_run+0x50>
  108981:	89 c3                	mov    %eax,%ebx
  108983:	eb d6                	jmp    10895b <debug_shell_run+0x54b>
  108985:	83 ec 0c             	sub    $0xc,%esp
  108988:	68 37 96 10 00       	push   $0x109637
  10898d:	e8 5e c0 ff ff       	call   1049f0 <serial_puts>
  108992:	83 c4 10             	add    $0x10,%esp
  108995:	eb c4                	jmp    10895b <debug_shell_run+0x54b>
  108997:	e8 54 d0 ff ff       	call   1059f0 <acpi_shutdown>
  10899c:	83 ec 0c             	sub    $0xc,%esp
  10899f:	68 20 a4 10 00       	push   $0x10a420
  1089a4:	e8 67 a0 ff ff       	call   102a10 <kpanic>
  1089a9:	83 c6 02             	add    $0x2,%esi
  1089ac:	e9 0a fe ff ff       	jmp    1087bb <debug_shell_run+0x3ab>
  1089b1:	e8 ba d0 ff ff       	call   105a70 <acpi_reboot>
  1089b6:	2e 8d b4 26 00 00 00 	lea    %cs:0x0(%esi,%eiz,1),%esi
  1089bd:	00 
  1089be:	66 90                	xchg   %ax,%ax

001089c0 <shell_task_fn>:
  1089c0:	56                   	push   %esi
  1089c1:	5e                   	pop    %esi
  1089c2:	83 ec 0c             	sub    $0xc,%esp
  1089c5:	e8 46 fa ff ff       	call   108410 <debug_shell_run>
  1089ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

001089d0 <debug_shell_init>:
  1089d0:	83 ec 14             	sub    $0x14,%esp
  1089d3:	68 73 96 10 00       	push   $0x109673
  1089d8:	68 c0 89 10 00       	push   $0x1089c0
  1089dd:	e8 ee d6 ff ff       	call   1060d0 <task_create>
  1089e2:	58                   	pop    %eax
  1089e3:	5a                   	pop    %edx
  1089e4:	68 88 a4 10 00       	push   $0x10a488
  1089e9:	6a 01                	push   $0x1
  1089eb:	e8 40 c1 ff ff       	call   104b30 <klog>
  1089f0:	83 c4 1c             	add    $0x1c,%esp
  1089f3:	c3                   	ret
