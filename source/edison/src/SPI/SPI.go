package SPI

import (
	"fmt"
	"log"
	"os"
	"syscall"
	"unsafe"
)

const (
	// SpiMajorVersion is the major version of SPI found in file /dev/spidev[MajorVersion].[MinorVersion]
	SpiMajorVersion = byte(5)
	// SpiMinorVersion is the minor version of SPI found in file /dev/spidev[MajorVersion].[MinorVersion]
	SpiMinorVersion = byte(1)

	// SPI constants
	spiIOCWrMode        = 0x40016B01
	spiIOCWrBitsPerWord = 0x40016B03
	spiIOCWrMaxSpeedHz  = 0x40046B04

	spiIOCRdMode        = 0x80016B01
	spiIOCRdBitsPerWord = 0x80016B03
	spiIOCRdMaxSpeedHz  = 0x80046B04

	spiIOCMessage0    = 1073769216 //0x40006B00
	spiIOCIncrementor = 2097152    //0x200000

	// Default SPI values
	defaultDelayMs  = 0
	defaultSPIBPW   = 8
	defaultSPISpeed = 100000
)

const (
	// SPI mode constants
	spiCpha = 0x01
	spiCpol = 0x02

	// SPIMode0 represents the mode0 operation (CPOL=0 CPHA=0) of spi.
	SPIMode0 = (0 | 0)

	// SPIMode1 represents the mode0 operation (CPOL=0 CPHA=1) of spi.
	SPIMode1 = (0 | spiCpha)

	// SPIMode2 represents the mode0 operation (CPOL=1 CPHA=0) of spi.
	SPIMode2 = (spiCpol | 0)

	// SPIMode3 represents the mode0 operation (CPOL=1 CPHA=1) of spi.
	SPIMode3 = (spiCpol | spiCpha)
)

type spiTransferData struct {
	txBuf uint64
	rxBuf uint64

	length            uint32
	speedHz           uint32
	delayMilliseconds uint16
	bitsPerWord       uint8
}

var initialized bool
var spiFile *os.File
var defaultSpiTransferData spiTransferData

func getSpiMessageCode(n uint32) uint32 {
	return (spiIOCMessage0 + (n * spiIOCIncrementor))
}

func writeFile(path string, data []byte) {
	file, err := os.OpenFile(path, os.O_WRONLY, 0644)
	defer file.Close()
	if err != nil {
		log.Fatal(err)
	}

	file.Write(data)
}

// InitializeSPI opens the SPI file and sets the default values for SPI communication
func InitializeSPI() {
	if initialized {
		return
	}

	// Export the GPIO pins necessary for SPI
	for _, pin := range []string{"111", "115", "114", "109", "263", "240", "262", "241", "242", "243", "258", "259", "260", "261", "226", "227", "228", "229", "214"} {
		writeFile("/sys/class/gpio/export", []byte(pin))
	}

	var directionMap = map[string]string{
		"214": "low",
		"263": "high",
		"240": "high",
		"262": "high",
		"241": "high",
		"242": "high",
		"243": "high",
		"258": "high",
		"259": "high",
		"260": "low",
		"261": "high",
		"226": "in",
		"227": "in",
		"228": "in",
		"229": "in",
	}

	// Configure pin directions
	for pinName, direction := range directionMap {
		writeFile("/sys/class/gpio/gpio"+pinName+"/direction", []byte(direction))
	}

	// Configure MOSI, MISO, SCK and SS pins.
	writeFile("/sys/kernel/debug/gpio_debug/gpio111/current_pinmux", []byte("mode1"))
	writeFile("/sys/kernel/debug/gpio_debug/gpio115/current_pinmux", []byte("mode1"))
	writeFile("/sys/kernel/debug/gpio_debug/gpio114/current_pinmux", []byte("mode1"))
	writeFile("/sys/kernel/debug/gpio_debug/gpio109/current_pinmux", []byte("mode1"))

	writeFile("/sys/class/gpio/gpio214/direction", []byte("high"))

	var err error
	if spiFile, err = os.OpenFile(fmt.Sprintf("/dev/spidev%v.%v", SpiMajorVersion, SpiMinorVersion), os.O_RDWR, os.ModeExclusive); err != nil {
		log.Fatal(err)
	}

	var mode = uint8(SPIMode0)
	SetMode(mode)

	defaultSpiTransferData = spiTransferData{}

	// Initialize SPI Settings
	SetSpeed(defaultSPISpeed)
	SetBitsPerWord(defaultSPIBPW)
	SetDelay(defaultDelayMs)

	initialized = true
}

// SetMode sets the SPI protocol mode
func SetMode(mode uint8) {
	_, _, errno := syscall.Syscall(syscall.SYS_IOCTL, spiFile.Fd(), spiIOCWrMode, uintptr(unsafe.Pointer(&mode)))
	if errno != 0 {
		err := syscall.Errno(errno)
		log.Fatal(err)
	}
}

// SetSpeed sets the SPI speed in HZ
func SetSpeed(speed uint32) {
	_, _, errno := syscall.Syscall(syscall.SYS_IOCTL, spiFile.Fd(), spiIOCWrMaxSpeedHz, uintptr(unsafe.Pointer(&speed)))
	if errno != 0 {
		err := syscall.Errno(errno)
		log.Fatal(err)
	}

	defaultSpiTransferData.speedHz = speed
}

// SetBitsPerWord sets the bitsPerWord for SPI calls
func SetBitsPerWord(bitsPerWord uint8) {
	_, _, errno := syscall.Syscall(syscall.SYS_IOCTL, spiFile.Fd(), spiIOCWrBitsPerWord, uintptr(unsafe.Pointer(&bitsPerWord)))
	if errno != 0 {
		err := syscall.Errno(errno)
		log.Fatal(err)
	}

	defaultSpiTransferData.bitsPerWord = bitsPerWord
}

// SetDelay sets the SPI delay
func SetDelay(delayMs uint16) {
	defaultSpiTransferData.delayMilliseconds = delayMs
}

// TransferAndReceiveData will transfer any data in dataBuffer over SPI and put the response in dataBuffer
func TransferAndReceiveData(dataBuffer []uint8) {
	if !initialized {
		InitializeSPI()
	}

	len := len(dataBuffer)
	dataCarrier := defaultSpiTransferData

	dataCarrier.length = uint32(len)
	dataCarrier.txBuf = uint64(uintptr(unsafe.Pointer(&dataBuffer[0])))
	dataCarrier.rxBuf = uint64(uintptr(unsafe.Pointer(&dataBuffer[0])))

	_, _, errno := syscall.Syscall(syscall.SYS_IOCTL, spiFile.Fd(), uintptr(getSpiMessageCode(1)), uintptr(unsafe.Pointer(&dataCarrier)))
	if errno != 0 {
		err := syscall.Errno(errno)
		log.Fatal(err)
	}
}

// ReceiveData receives n bytes of data
func ReceiveData(length int) []uint8 {
	data := make([]uint8, length)
	TransferAndReceiveData(data)
	return data
}

// Close closes the SPI file
func Close() {
	if initialized {
		return
	}
	spiFile.Close()
}
