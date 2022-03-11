#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MEM_SIZE 16384  // MUST equal PAGE_SIZE * PAGE_COUNT
#define PAGE_SIZE 256  // MUST equal 2^PAGE_SHIFT
#define PAGE_COUNT 64
#define PAGE_SHIFT 8  // Shift page number this much

// Simulated RAM
unsigned char mem[MEM_SIZE];

// Get the page table for a given process
unsigned char get_page_table(int proc_num)
{
    return mem[proc_num + 64];
}

// Convert a page,offset into an address

int get_address(int page, int offset)
{
    return (page << PAGE_SHIFT) | offset;
}

// Initialize RAM

void initialize_mem(void)
{
    // Zero every byte of physical memory in the mem array.
    memset(mem, 0, MEM_SIZE);
    // Mark zero page as "used" in the Used Page Table. (That is, set mem[0] to 1.)
    mem[0] = 1;
}

// Allocate a physical page
// Returns the number of the page, or 0xff if no more pages available

unsigned char get_page(void)
{
    // Allocate pages for a new process
    // For each page_number in the Used Page array in zero page:
    for (int page_number = 0; PAGE_COUNT >= page_number; page_number++)
    {
        // If it's unused (if it's 0):
        if (mem[page_number] == 0)
        {
            // mem[page_number] = 1 // mark used
            mem[page_number] = 1;
            // return the page_number
            return page_number;
        }
    }
    return 0xff;  // indicating no free pages
}
// This includes the new process page table and page_count data pages.

void new_process(int proc_num, int page_count)
{
//  Get the page table page
    int page_table = get_page();
// Set this process's page table pointer in zero page
    mem[64 + proc_num] = page_table;
//  Allocate data pages
//  For i from 0 to page_count:
    for (int i = 0; i < page_count; i++)
    {
        // new_page = AllocatePage()
        int new_page = get_page();
        // page_table = mem[i]->new_page; Set the page table to map virt -> phys
        // Virtual page number is i
        // Physical page number is new_page
        int page_table_addr = get_address(page_table, i);
        mem[page_table_addr] = new_page;
    }
}

// DeallocatePage(p):
void deallocate_page(int page_number)
{
//  Set the value at address p in zeropage to 0
    mem[page_number] = 0;
}

// KillProcess(p):
//     Get the page table page for this process

//     Get the page table for this process
// page_table_page = mem[proc_num + 64];

//     For each entry in the page table
//         If it's not 0:
//             Deallocate that page

//     Deallocate the page table page

// GetPhysicalAddress(proc_num, virtual_addr):
//     Get the virtual page (see code above)
//     Get the offset
//     Get the physical page from the page table

//     Build the physical address from the phys page and offset

//     Return it

// StoreValue(proc_num, virt_addr, value):
//     phys_addr = GetPhysicalAddr(proc_num, virt_addr)
//     mem[phys_addr] = value

// LoadValue(proc_num, virt_addr):
//     phys_addr = GetPhysicalAddr(proc_num, virt_addr)
//     value = mem[phys_addr]

// Print the free page map
void print_page_free_map(void)
{
    printf("--- PAGE FREE MAP ---\n");

    for (int i = 0; i < 64; i++) {
        int addr = get_address(0, i);

        printf("%c", mem[addr] == 0? '.': '#');

        if ((i + 1) % 16 == 0)
            putchar('\n');
    }
}

// Print the address map from virtual pages to physical
void print_page_table(int proc_num)
{
    printf("--- PROCESS %d PAGE TABLE ---\n", proc_num);

    // Get the page table for this process
    int page_table = get_page_table(proc_num);

    // Loop through, printing out used pointers
    for (int i = 0; i < PAGE_COUNT; i++) {
        int addr = get_address(page_table, i);

        int page = mem[addr];

        if (page != 0) {
            printf("%02x -> %02x\n", i, page);
        }
    }
}

// Main -- process command line
int main(int argc, char *argv[])
{
    assert(PAGE_COUNT * PAGE_SIZE == MEM_SIZE);

    if (argc == 1) {
        fprintf(stderr, "usage: ptsim commands\n");
        return 1;
    }
    
    initialize_mem();

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "np") == 0) {
            int proc_num = atoi(argv[++i]);
            int pages = atoi(argv[++i]);
            new_process(proc_num, pages);
        }
        else if (strcmp(argv[i], "pfm") == 0) {
            print_page_free_map();
        }
        else if (strcmp(argv[i], "ppt") == 0) {
            int proc_num = atoi(argv[++i]);
            print_page_table(proc_num);
        }
    }
}
