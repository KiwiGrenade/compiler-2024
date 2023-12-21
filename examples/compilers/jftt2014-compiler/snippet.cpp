#include "snippet.hpp"
#include "label.hpp"
#include "var_block.hpp"
#include "definitions.hpp"
#include "instruction.hpp"

snippet::snippet() : code_block(CB_SNIPPET)
	{ can_be_flattened = true; }
snippet::snippet(ptr(code_block) cb) : code_block(CB_SNIPPET)
	{ can_be_flattened = true; blocks.push_back(cb); }
snippet::~snippet() { }

std::deque<std::string>
	snippet::render(size_t start)
{
	std::deque<std::string> result;
	for(auto it = blocks.begin(); it != blocks.end(); ++it)
	{
		std::deque<std::string> sub = (*it)->render(start);
		result.insert(result.end(), sub.begin(), sub.end());
		start += sub.size();
	}
	return result;
}

void
	snippet::flatten()
{
	std::deque<ptr(code_block)> newblocks;

	for(auto it = blocks.begin(); it != blocks.end(); ++it)
	{
		if((*it)->can_be_flattened)
		{
			ptr(snippet) pt = std::dynamic_pointer_cast<snippet>(*it);
			pt->flatten();
			newblocks.insert(newblocks.end(),
							pt->blocks.begin(), pt->blocks.end());
		}
		else
			newblocks.push_back(*it);
	}

	blocks = newblocks;
}

std::multiset<ident>
    snippet::used_idents()
{
    std::multiset<ident> result;
    for(auto &it : blocks)
    {
        std::multiset<ident> uid = it->used_idents();
        for(auto &id : uid)
            result.insert(id);
    }
    return result;
}

std::multiset<ident>
    snippet::used_labels()
{
    std::multiset<ident> result;
    for(auto &it : blocks)
    {
        std::multiset<ident> uid = it->used_labels();
        for(auto &id : uid)
            result.insert(id);
    }
    return result;
}

void
    snippet::remove_unused_labels(std::multiset<ident> used)
{
    if(used.empty()) // default case
        used = used_labels();
    
    for(auto it = blocks.begin(); it != blocks.end(); ++it)
    {
        std::shared_ptr<snippet> snip;
        std::shared_ptr<label> lbl;
        if(snip = std::dynamic_pointer_cast<snippet>(*it))
        {
            snip->remove_unused_labels(used);
        }
        else if(lbl = std::dynamic_pointer_cast<label>(*it))
        {
            if(!used.count(lbl->get_name())) // Unused
            {
                info("Removing unused label " << lbl->get_name());
                it = blocks.erase(it); 
                if(it == blocks.end()) return;
            }
        }
    }
}

void
	snippet::optimize(const std::multiset<ident> &preceders, const std::multiset<ident> &following)
{
    if(preceders.empty() && following.empty())
    {
        logme(" [OPT] Master level opt");
        // We are on program level, so try the "master" optimization
        std::multiset<ident> pre;
        std::multiset<ident> post(used_idents());
        info("[OPT] Master block has " << blocks.size() << " blocks with " << post.size() << " idents");
        
        remove_unused_labels(); //prep
        
        for(auto &blck: blocks)
        {
            bool done;
            std::multiset<ident> current = blck->used_idents();
            printset(current);
            printset(post);
            for(auto &in : current) post.erase(post.find(in));
            
            if(post.empty() && pre.empty())
            // Try allowing another master-level opt
            {
                // Force in-block opt
                blck->optimize(pre, post);
            }
            else            
                do
                {
                    done = false;
                    
                    // Find variables that:
                    // var1 in current, var1 not in following (var1 may be in preceders);
                    // var2 in following, var2 not in current, var2 not in preceders
                    std::multiset<ident> tvar1;
                    for(auto &it : current)
                    {
                        if(!post.count(it)) tvar1.insert(it);
                    }
                    std::multiset<ident> tvar2;
                    for(auto &it : post)
                    {
                        if(!current.count(it) && !pre.count(it)) tvar2.insert(it);
                    }
                    std::deque<ident> var1 = multi_to_sorted(tvar1);
                    std::deque<ident> var2 = multi_to_sorted(tvar2);
                    
                    logme(" [OPT] Following " << post.size());
                    printset(post);
                    logme(" [OPT] Current " << current.size());
                    printset(current);
                    logme(" [OPT] Preceders has " << pre.size());
                    printset(pre);
                    
                    logme(" [OPT] Var1 has " << var1.size());
                    printset(var1);
                    logme(" [OPT] Var2 has " << var2.size());
                    printset(var2);
                    
                    if(!var1.empty() && !var2.empty())
                    {
                        ident v1 = *(var1.begin()), v2 = *(var2.begin());
                        ident vNew = v1 + "/" + v2;
                        rename_ident(vNew, v1);
                        rename_ident(vNew, v2);
    //                     var1.erase(v1);
    //                     var2.erase(v2);
                        // most of those should be done 0 times...
                        for(size_t num = pre.erase(v1); num; num--) pre.insert(vNew);
                        for(size_t num = pre.erase(v2); num; num--) pre.insert(vNew);
                        for(size_t num = current.erase(v1); num; num--) current.insert(vNew);
                        for(size_t num = current.erase(v2); num; num--) current.insert(vNew);
                        for(size_t num = post.erase(v1); num; num--) post.insert(vNew);
                        for(size_t num = post.erase(v2); num; num--) post.insert(vNew);
                        
                        info("[OPT] Substitued `" << vNew << "` for `" << v1 << "` and `" << v2 << "`");
                        
                        done = true;
                    }
                }
                while(done);
               
            // Moving to the next block, post is cleared at the beginning of for block
            pre.insert(current.begin(), current.end());
        }
        
        // Flatten the layer, cause if/while/assign blocks are no longer needed
        flatten();
        
        remove_unused_labels(); // just checking
        
        // Look for pure jumps to self:
        auto it1 = blocks.begin(), it2 = it1++;
        int i = 0;
        for(; it1 != blocks.end(); ++it1, ++i)
        {
            //check if it1 is an label
            std::shared_ptr<label> lbl;
            if(lbl = std::dynamic_pointer_cast<label>(*it1))
            {
                //logme(" [OPT] Found an label! " << lbl->get_name());
                //check if it2 is an instruction)
                std::shared_ptr<instruction> instr;
                for(it2 = it1 - 1; it2 != blocks.begin() && 
                    !(instr = std::dynamic_pointer_cast<instruction>(*it2)); --it2);
                    // go back with it2 until you reach first instruction befire it2
                if(instr) // if found (not null)
                {
                    //if it is, check if it's an JUMP to lbl:
                    if(instr->peek() == "JUMP " + lbl->get_name())
                    {
                        info("[OPT] Found an empty jump label " << instr->peek());
                        // now we can remove the empty JUMP (the label may still be in use btw:
                        it1 = blocks.erase(it2); //after ++it1 it shold jump to the next available (label)
                        ++i;
                    }
                }
            }
        }
        
        remove_unused_labels(); // Empty jumps anyone?
        
        // Look for indirect jumps (jumps to jump label), and similar optimization issues
        for(it1 = blocks.begin(), i = 0; it1 != blocks.end(); ++it1, ++i)
        {
            //check if it1 is an instruction
            ptr(instruction) in1, in2;
            if(in1 = std::dynamic_pointer_cast<instruction>(*it1))
            {
                if(in1->peek().find("STORE") == 0)
                {
                    logme("Found STORE instruction " << in1->peek());
					it2 = it1;
                    ++it2;
					bool temp;
                    for(temp = true; temp && it2 != blocks.end(); it2++)
                    {
                        if(in2 = std::dynamic_pointer_cast<instruction>(*it2))
                        {
                            logme("Followed by " << in2->peek());
                            temp = false; // Assume it's the end
                            if(in2->peek().find("LOAD") == 0)  // checking store a, load a case
                            {
                                if(in1->peek().substr(6) == in2->peek().substr(5))
                                {
                                    info("[OPT] Found " << in1->peek() << "/" << in2->peek() << " pair");
                                    // if so, we can omit the LOAD, but the store may still be neccessary
                                    it2 = blocks.erase(it2);
                                    --it2;
                                    temp = true; // I'm quite sure that's impossible
									break;
                                }
                            }
                        }
                        else if(std::dynamic_pointer_cast<label>(*it2))
                            temp = false; // Skip others and try again
                    }
					if(temp && it2 != blocks.end()) // broken loop
					{
						// restart scan
						it1 = blocks.begin();
						continue; // will start at it+1, but damn it, whatever
					}
                    
                    // try scanning until another STORE/LOAD/ADD/SUB/PRINT/SCAN a is found, or
                    // label | JUMP/JZ/JG/JODD
                    ident lbl = in1->peek().substr(6);
					it2 = it1;
                    for(++it2; it2 != blocks.end(); ++it2)
                    {
                        if(in2 = std::dynamic_pointer_cast<instruction>(*it2))
                        {
                            if(in2->peek() == "STORE " + lbl || in2->peek() == "SCAN " + lbl)
                            {
                                // STORE a after STORE a means we don't really need STORE here...
								info("erasing " <<in1->peek() << " at " << it1 - blocks.begin() << " by " << in2->peek() << " at " << it2 - blocks.begin());
                                it1 = blocks.erase(it1);
                                it2 = blocks.begin(); // reset iterator
                                info("[OPT] Unnecessary " << in1->peek() << " found 1");
                                break;
                            }
                            else if(in2->peek() == "LOAD " + lbl || in2->peek() == "SUB " + lbl || 
                                in2->peek() == "ADD " + lbl || in2->peek() == "PRINT " + lbl || 
                                in2->peek().find("JG") == 0 || in2->peek().find("JZ") == 0 || 
                                in2->peek().find("JUMP") == 0 || in2->peek().find("JODD") == 0)
                                break;
                        }
                        if(std::dynamic_pointer_cast<label>(*it2)) break;
                    }
                    if(it2 == blocks.end() && !blocks.empty()) // this means lbl was not used after storing, so the STORE lbl is not really necessary...
                    {
                        it1 = blocks.erase(it1);
                        info("[OPT] Unnecessary " << in1->peek() << " found 2");
                    }
                }
                else if(in1->peek().find("LOAD") == 0)
                {
                    logme("Found LOAD instruction " << in1->peek());
                    ++(it2 = it1);
                    for(bool temp = true; temp && it2 != blocks.end(); it2++)
                    {
                        if(in2 = std::dynamic_pointer_cast<instruction>(*it2))
                        {
                            logme("Followed by " << in2->peek());
                            temp = false; // Assume it's the end
                            if(in2->peek().find("STORE") == 0)  // checking load a, store a case
                            {
                                if(in1->peek().substr(5) == in2->peek().substr(4))
                                {
                                    info("[OPT] Found " << in1->peek() << "/" << in2->peek() << " pair");
                                    // if so, we can omit the STORE, but the load may still be neccessary
                                    it2 = blocks.erase(it2);
                                    --it2;
                                    temp = true; // I'm quite sure that's impossible
                                }
                            }
                            else if(in2->peek().find("LOAD") == 0)  // checking load a, load b case
                            {
                                if(in1->peek().substr(5) == in2->peek().substr(4))
                                {
                                    info("[OPT] Found " << in1->peek() << "/" << in2->peek() << " pair");
                                    // if so, we can omit the first load
                                    it1 = blocks.erase(it1);
                                }
                            }
                        }
                        else if(std::dynamic_pointer_cast<label>(*it2))
                            temp = false; // Skip others and try again
                    }
                }
                else if(in1->peek().find("JUMP") == 0 || in1->peek().find("JODD") == 0 || in1->peek().find("JZ") == 0 || in1->peek().find("JG") == 0)
                {
                    logme("Found JUMP instruction " << in1->peek());
                    // Find it's TO label:
                    ptr(label) lbto;
                    ident to = in1->peek().substr(in1->peek().find(" ") + 1);
                    logme(" Scanning for label...");
                    for(auto it2 = blocks.begin(); it2 != blocks.end(); ++it2)
                    {
                        if(lbto = std::dynamic_pointer_cast<label>(*it2))
                        {
                            if(lbto->get_name() == to)
                            {
                                logme("\t... found.");
                                // find the next instruction:
                                for(++it2;it2 != blocks.end();)
                                {
                                    if(in2 = std::dynamic_pointer_cast<instruction>(*it2))
                                    {
                                        if(in2->peek().find("JUMP") == 0)
                                        {
                                            info("[OPT] Found JUMP to JUMP case " << in2->peek() << " <- " << in1->peek());
                                            in1->rename_ident(in2->peek().substr(4), to);
                                            info("[OPT] Renamed to: " << in1->peek());
                                        }
                                        it2 = blocks.end();
                                    }
                                    else
                                        ++it2;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            
            if(it1 == blocks.end()) break; // force breaking for for end++
        }
        
        remove_unused_labels(); // More empty labels?
        
        // remove last lines till the last jump/lbl/print
        for(auto it = blocks.end() - 1; it != blocks.begin(); --it)
        {
            std::shared_ptr<label> lbl;
            std::shared_ptr<instruction> in;
            
            if(in = std::dynamic_pointer_cast<instruction>(*it))
            {
                if(in->peek().find("PRINT") == 0 || in->peek().find("SCAN") == 0 || 
                   in->peek().find("JG")    == 0 || in->peek().find("JZ")   == 0 || 
                   in->peek().find("JUMP")  == 0 || in->peek().find("JODD") == 0)
                   break;
                info("[OPT] Trimming instruction " << in->peek());
                blocks.erase(it);
                if(blocks.empty())
                {
                    info("[OPT] Optimized to nothing");
                    return; // nothing to do, optimized to nothing
                }
                it = blocks.end(); // will -- soon
            }
            else if(lbl = std::dynamic_pointer_cast<label>(*it))
            {
                // maybe try to "jump back" with halt?
                break;
            }
        }
        
        remove_unused_labels(); // Shouldn't change anything, but...
        
        // TODO: try to reconstruct MASTER-LEVEL structure with blocks around same jump-label
        // regions to run merging variables once again
    }
}

void
	snippet::rename_ident(ident to, ident from)
{
	for(auto &blck : blocks)
		blck->rename_ident(to, from);
}

ptr(snippet)
	snippet::build_assign_num(ident dest, uint64 arg)
{
	ptr(snippet) __, _(new snippet());
    logme("Created snippet for assign num " << arg << " to " << dest);
    uint64 tmp = arg;
    
    //*
	while(arg)
	{
		if(arg & 1)
			_PUSH_ASM_FRONT("INC");
        arg >>= 1;
		if(arg)
            _PUSH_ASM_FRONT("SHL");
	}
	_PUSH_ASM_FRONT("ZERO");
    if(dest != "ACC")
        _PUSH_ASMa("STORE", dest);
    
    __ = _; _ = std::make_shared<snippet>();
    //*/
    _PUSH_ASM("ZERO");
    uint64 mask = 0xc000000000000000;
    int memory = 0;
    bool nonzero = false;
    uint64 val; arg = tmp;
    for(int i = 64; i > 0; --i)
    {
        val = (arg & mask) >> 62;
        switch(val)
        {
            case 0:
                {
                    if(nonzero) _PUSH_ASM("SHL"); 
                    memory = 0;
                    break;
                }
            case 1:
                { 
                    if(nonzero) _PUSH_ASM("SHL"); 
                    memory = 0;
                    break;
                }
            case 2:
                if(memory == 0)
                { 
                    if(nonzero) _PUSH_ASM("SHL"); 
                    _PUSH_ASM("INC"); nonzero = true;
                    memory = 1;
                    break; 
                }    
                if(memory == 1)
                { 
                    _PUSH_ASM("SHL"); 
                    _PUSH_ASM("DEC");
                    break;
                }    
            case 3:
                if(memory == 0)
                { 
                    _PUSH_ASM("INC"); nonzero = true;
                    _PUSH_ASM("SHL"); 
                    memory = 1;
                    break; 
                }    
                if(memory == 1)
                { 
                    _PUSH_ASM("SHL"); 
                    break;
                }    
        }
        arg <<= 1;
    }
    if(dest != "ACC")
        _PUSH_ASMa("STORE", dest);
    //*/
    // Select the cheaper way:
    return _->blocks.size() < __->blocks.size() ? _ : __;
}

ptr(snippet)
	snippet::build_assign(ident dest, ident arg)
{
    ptr(snippet) _(new snippet());
    logme("Created snippet for assign var/const");
    
    if(constants.count(arg)) // const assign is faster via assign_num:
        return build_assign_num(dest, constants[arg]);
    
    if(dest == arg) return _; // do nothing!

    _PUSH_ASMa("LOAD", arg);
    _PUSH_ASMa("STORE", dest);

    return _;
}

ptr(snippet)
    snippet::build_add(ident dest, ident arg1, ident arg2)
{
    ptr(snippet) _(new snippet());
    
    // comp to const:
    if(constants.count(arg1) || constants.count(arg2))
    {
        // both?
        if(constants.count(arg1) && constants.count(arg2))
        {
            logme("Constant addition");
            return build_assign_num(dest, constants[arg1] + constants[arg2]);
        }

        ident cst = constants.count(arg1) ? arg1 : arg2;
        ident var = constants.count(arg1) ? arg2 : arg1;
        uint64 csv = constants[cst];

        logme("Semi-constant addition");
        if(csv < 100) // should be still better
        {
            _PUSH_ASMa("LOAD", var);
            for(; csv > 0; --csv) _PUSH_ASM("INC");
            _PUSH_ASMa("STORE", dest);
            return _;
        }
    }

    logme("Created snippet for add " << arg1 << " " << arg2);
    _PUSH_ASMa("LOAD", arg1);
    _PUSH_ASMa("ADD", arg2);
    _PUSH_ASMa("STORE", dest);
    return _;
}
ptr(snippet)
    snippet::build_sub(ident dest, ident arg1, ident arg2)
{
    ptr(snippet) _(new snippet());
    
    // comp to const:
    if(constants.count(arg1) || constants.count(arg2))
    {
        // both?
        if(constants.count(arg1) && constants.count(arg2))
        {
            logme("Constant subtraction");
            return build_assign_num(dest, constants[arg1] - constants[arg2] < 0 ? 0 : constants[arg1] - constants[arg2]);
        }

        ident cst = constants.count(arg1) ? arg1 : arg2;
        ident var = constants.count(arg1) ? arg2 : arg1;
        uint64 csv = constants[cst];

        logme("Semi-constant subtraction");
        if(cst == arg1)
        {
            _PUSH_BLOCK(build_assign_num("ACC", csv));
            _PUSH_ASMa("SUB", arg2);
            _PUSH_ASMa("STORE", dest);
            return _;
        }
        else if(csv < 100) // should be still better
        {
            _PUSH_ASMa("LOAD", arg1);
            for(; csv > 0; --csv) _PUSH_ASM("DEC");
            _PUSH_ASMa("STORE", dest);
            return _;
        }
    }

    logme("Created snippet for sub " << arg1 << " " << arg2);
    _PUSH_ASMa("LOAD", arg1);
    _PUSH_ASMa("SUB", arg2);
    _PUSH_ASMa("STORE", dest);
    return _;
}
ptr(snippet)
    snippet::build_mul(ident dest, ident arg1, ident arg2)
{
    ptr(snippet) _(new snippet());

    // comp to const:
    if(constants.count(arg1) || constants.count(arg2))
    {
        // both?
        if(constants.count(arg1) && constants.count(arg2))
        {
			logme("Constant multiplication");
            return build_assign_num(dest, constants[arg1] * constants[arg2]);
        }

        ident cst = constants.count(arg1) ? arg1 : arg2;
        ident var = constants.count(arg1) ? arg2 : arg1;
        uint64 csv = constants[cst];

        switch(csv)
        {
            case 0:
                return build_assign_num(dest, 0);
            case 1:
                return build_assign(dest, var);
            default:
            {
				uint64 ptr = 1;
				// Shift left until ptr >= csv
				while(csv >= ptr) ptr <<= 1;
				// go back 1 digit
				ptr >>= 1;
				
				// "Zero, shift left, add" :
                _PUSH_ASMa("LOAD", var);
                //_PUSH_ASM("SHL");
				ptr >>= 1;
				while(ptr) // while pointer "in range"
                {
                    _PUSH_ASM("SHL");
                    if(csv & ptr)
					{
                        _PUSH_ASMa("ADD", var);
					}
					ptr >>= 1;
                }
                _PUSH_ASMa("STORE", dest);
                return _;
            }
        }
    }

    _DECL_LBL(Lmul_start);
    _DECL_LBL(Lmul_p0);
    _DECL_LBL(Lmul_p1);
    _DECL_LBL(Lmul_zero);
    _DECL_LBL(Lmul_end);

    _PUSH_VAR(Vt1);
    _PUSH_VAR(Vt2);
    _PUSH_VAR(Vt3);
    
    ident td;
    if(use_temp_dest)
    {
        td = dest; dest = Vt3;
    }

    ident src1, src2;
    // If dest == arg1 then swap Vt2 with Vt1, so loaded Vt1 can stay in ACC
    if(dest == arg1)
        (src1 = arg2), (src2 = arg1);
    else
        (src1 = arg1), (src2 = arg2);
    

    bool use_zero = false;
    if(dest == arg1 && dest == arg2) // cant zero result before assign
    {
        _PUSH_BLOCK(build_assign(Vt2, src2)); // Vt2 in ACC
        if(!no_jzero){ _PUSH_ASMe("JZ", Lmul_end); }
        _PUSH_BLOCK(build_assign(Vt1, src1)); // Vt1 in ACC
        _PUSH_BLOCK(build_assign_num(dest, 0));
        _PUSH_ASMa("LOAD", Vt1);
    }
    else if(arg1 == arg2) 
    {
        _PUSH_BLOCK(build_assign(Vt2, src2)); // Vt2 in ACC
        if(!no_jzero){ _PUSH_ASMe("JZ", Lmul_zero); use_zero = true; }
        _PUSH_ASMa("STORE", Vt1); // Vt1 in ACC
        _PUSH_BLOCK(build_assign_num(dest, 0));
    }
    else
    {
        _PUSH_BLOCK(build_assign(Vt2, src2)); // Vt2 in ACC
        if(!no_jzero){ _PUSH_ASMe("JZ", Lmul_zero); use_zero = true; }
        _PUSH_BLOCK(build_assign_num(dest, 0));
        _PUSH_BLOCK(build_assign(Vt1, src1)); // Vt1 in ACC
        if(!no_jzero){ _PUSH_ASMe("JZ", Lmul_end); }
    }
    _PUSH_LBL(Lmul_start);
    _PUSH_ASMe("JODD", Lmul_p1);
    _PUSH_ASMe("JUMP", Lmul_p0);
    _PUSH_LBL(Lmul_p1);
    _PUSH_BLOCK(build_add(dest, dest, Vt2));
    _PUSH_LBL(Lmul_p0);
    _PUSH_ASMa("LOAD", Vt2);
    _PUSH_ASM("SHL");
    _PUSH_ASMa("STORE", Vt2);
    _PUSH_ASMa("LOAD", Vt1);
    _PUSH_ASM("SHR");
    _PUSH_ASMa("STORE", Vt1); // Vt1 in ACC
    _PUSH_ASMe("JG", Lmul_start);
    if(use_zero)
    {
        _PUSH_ASMe("JUMP", Lmul_end);
        _PUSH_LBL(Lmul_zero);
        _PUSH_ASM("ZERO");
        _PUSH_ASMa("STORE", dest); // dest in ACC
    }
    _PUSH_LBL(Lmul_end);
    
    if(use_temp_dest) _PUSH_BLOCK(build_assign(td, dest)); // Vt1 in ACC    
    return _;
}
ptr(snippet)
    snippet::build_div(ident dest, ident arg1, ident arg2)
{
    ptr(snippet) _(new snippet());

    // comp to const:
    if(constants.count(arg1) || constants.count(arg2))
    {
        // both?
        if(constants.count(arg1) && constants.count(arg2))
        {
			logme("Constant division");
            return build_assign_num(dest, constants[arg2] ? constants[arg1] / constants[arg2] : 0);
        }

        if(constants.count(arg2))
        {
            switch(constants[arg2])
            {
                case 0:
                    return build_assign_num(dest, 0);
                case 1:
                    return build_assign(dest, arg1);
            }
            if(is_power_of_2(constants[arg2]))
            {
				logme("Constant division by 2^n");
                _PUSH_ASMa("LOAD", arg1);
                for(int x = constants[arg2] >> 1; x > 0; x >>= 1)
                    _PUSH_ASM("SHR");
                _PUSH_ASMa("STORE", dest);
				return _;
            }
        }
    }

	_PUSH_VAR(Vt1);
	_PUSH_VAR(Vt2);
	_PUSH_VAR(Vt3);
	_PUSH_VAR(Vt4);
    
    ident td;
    if(use_temp_dest)
    {
        td = dest; dest = Vt4;
    }

	_DECL_LBL(Ldiv_p0);
	_DECL_LBL(Ldiv_p1);
	_DECL_LBL(Ldiv_p2);
	_DECL_LBL(Ldiv_pre);
	_DECL_LBL(Ldiv_cond);
	_DECL_LBL(Ldiv_end);
	_DECL_LBL(Ldiv_zero);

    bool use_zero = false;
    if(dest == arg2) // cant zero result before assign
    {
        _PUSH_BLOCK(build_assign(Vt2, arg2)); // Vt2 in ACC
        _PUSH_ASMe("JZ", Ldiv_end); // this one has to stay (div by 0)
        _PUSH_ASMa("STORE", Vt3); // Vt2 in ACC
        _PUSH_BLOCK(build_assign(Vt1, arg1)); // Vt1 in ACC
		if(dest == arg1)
		{
            if(!no_jzero){ _PUSH_ASMe("JZ", Ldiv_end); }
		}
		else
		{
            if(!no_jzero){ _PUSH_ASMe("JZ", Ldiv_zero); use_zero = true; }
		}
		_PUSH_BLOCK(build_assign_num(dest, 0));
        _PUSH_ASMa("LOAD", Vt2);
    }
    else
    {
        _PUSH_BLOCK(build_assign(Vt1, arg1)); // Vt1 in ACC
        if(!no_jzero){ _PUSH_ASMe("JZ", Ldiv_zero); use_zero = true; }
        _PUSH_BLOCK(build_assign_num(dest, 0));
        _PUSH_BLOCK(build_assign(Vt2, arg2)); // Vt2 in ACC
        _PUSH_ASMe("JZ", Ldiv_end); // this one has to stay (div by 0)
        _PUSH_ASMa("STORE", Vt3); // Vt2 in ACC
    }

    _PUSH_ASMa("SUB", Vt1);   // If Vt2 - Vt1 > 0, then
                              // Vt2 > Vt1
	_PUSH_ASMe("JG", Ldiv_end);

	_PUSH_ASMa("LOAD", Vt2);   // Vt1 in ACC
    _PUSH_LBL(Ldiv_p0);
	_PUSH_ASMa("SUB", Vt1); // Vt1 - Vt2
	_PUSH_ASMe("JG", Ldiv_pre);
	_PUSH_ASMa("LOAD", Vt2);
	_PUSH_ASM("SHL"); // << Vt2
	_PUSH_ASMa("STORE", Vt2); // Vt2 still in ACC
	_PUSH_ASMe("JUMP", Ldiv_p0);

	_PUSH_LBL(Ldiv_pre);
	_PUSH_ASMa("LOAD", Vt2);
	_PUSH_ASM("SHR"); // >> Vt2
	_PUSH_ASMa("STORE", Vt2);
	_PUSH_ASMe("JUMP", Ldiv_cond);

	_PUSH_LBL(Ldiv_p1);
	_PUSH_ASMa("LOAD", Vt1);
	_PUSH_ASMa("SUB", Vt2);
	_PUSH_ASMa("STORE", Vt1);

	_PUSH_ASMa("LOAD", dest);
	_PUSH_ASM("SHL");
	_PUSH_ASM("INC");
	_PUSH_ASMa("STORE", dest);

	_PUSH_LBL(Ldiv_p2);
	_PUSH_ASMa("LOAD", Vt2);
	_PUSH_ASMa("SUB", Vt3);
	_PUSH_ASMe("JZ", Ldiv_end); // end
	_PUSH_ASMa("LOAD", Vt2);
	_PUSH_ASM("SHR"); // >> Vt2
	_PUSH_ASMa("STORE", Vt2);

	_PUSH_LBL(Ldiv_cond);
	_PUSH_ASMa("LOAD", Vt2);
	_PUSH_ASMa("SUB", Vt1); // Vt2 - Vt1 <= 0 -> Vt1 >= Vt2
	_PUSH_ASMe("JZ", Ldiv_p1);
	_PUSH_ASMa("LOAD", dest);
	_PUSH_ASM("SHL");
	_PUSH_ASMa("STORE", dest);
	_PUSH_ASMe("JUMP", Ldiv_p2);

    if(use_zero)
    {
        _PUSH_LBL(Ldiv_zero);
        _PUSH_ASM("ZERO");
        _PUSH_ASMa("STORE", dest); // dest in ACC
    }
    _PUSH_LBL(Ldiv_end);
    if(use_temp_dest) _PUSH_BLOCK(build_assign(td, dest)); 

    return _;
}
ptr(snippet)
    snippet::build_mod(ident dest, ident arg1, ident arg2)
{
    ptr(snippet) _(new snippet());

    // comp to const:
    if(constants.count(arg1) || constants.count(arg2))
    {
        // both?
        if(constants.count(arg1) && constants.count(arg2))
        {
            return build_assign_num(dest, constants[arg2] ? constants[arg1] % constants[arg2] : 0);
        }

        if(constants.count(arg2))
        {
            switch(constants[arg2])
            {
                case 0:
                    return build_assign_num(dest, 0);
                case 1:
					_DECL_LBL(Lmod2_0);
					_DECL_LBL(Lmod2_1);
					_DECL_LBL(Lmod2_end);
					_PUSH_ASMa("LOAD", arg1);
					_PUSH_ASMe("JODD", Lmod2_1);
					_PUSH_LBL(Lmod2_0);
					_PUSH_ASM("ZERO");
					_PUSH_ASMe("JUMP", Lmod2_end);
					_PUSH_LBL(Lmod2_1);
					_PUSH_ASM("ZERO");
					_PUSH_ASM("INC");
					_PUSH_LBL(Lmod2_end);
					_PUSH_ASMa("STORE", dest);
                    return _;
            }
        }
    }
    
	_PUSH_VAR(Vt1);
    _PUSH_VAR(Vt2);
    _PUSH_VAR(Vt3);
	//_PUSH_VAR(Vt4);

    if(!use_temp_dest)
        Vt1 = dest;

	_DECL_LBL(Lmod_p0);
	_DECL_LBL(Lmod_p1);
	_DECL_LBL(Lmod_p2);
	_DECL_LBL(Lmod_pre);
	_DECL_LBL(Lmod_cond);
	_DECL_LBL(Lmod_end);
	_DECL_LBL(Lmod_zero);

    bool use_zero = false;
    if(dest == arg2) // cant zero result before assign
    {
        _PUSH_BLOCK(build_assign(Vt2, arg2)); // Vt2 in ACC
        _PUSH_ASMe("JZ", Lmod_end);
        _PUSH_ASMa("STORE", Vt3); // Vt2 in ACC
		_PUSH_BLOCK(build_assign(Vt1, arg1)); // Vt1/dest in ACC
        if(!no_jzero){ _PUSH_ASMe("JZ", Lmod_end); }
		_PUSH_ASMa("LOAD", Vt2);
    }
    else
    {
        _PUSH_BLOCK(build_assign(Vt1, arg1)); // Vt1 in ACC
        if(!no_jzero){ _PUSH_ASMe("JZ", Lmod_zero); use_zero = true; }
        _PUSH_BLOCK(build_assign(Vt2, arg2)); // Vt2 in ACC
        _PUSH_ASMe("JZ", Lmod_zero); use_zero = true;
        _PUSH_ASMa("STORE", Vt3); // Vt2 in ACC
    }

    _PUSH_ASMa("SUB", Vt1);   // If Vt2 - Vt1 > 0, then
                              // Vt2 > Vt1
	_PUSH_ASMe("JG", Lmod_end);

	_PUSH_ASMa("LOAD", Vt2);   // Vt1 in ACC
    _PUSH_LBL(Lmod_p0);
	_PUSH_ASMa("SUB", Vt1); // Vt1 - Vt2
	_PUSH_ASMe("JG", Lmod_pre);
	_PUSH_ASMa("LOAD", Vt2);
	_PUSH_ASM("SHL"); // << Vt2
	_PUSH_ASMa("STORE", Vt2); // Vt2 still in ACC
	_PUSH_ASMe("JUMP", Lmod_p0);

	_PUSH_LBL(Lmod_pre);
	_PUSH_ASMa("LOAD", Vt2);
	_PUSH_ASM("SHR"); // >> Vt2
	_PUSH_ASMa("STORE", Vt2);
	_PUSH_ASMe("JUMP", Lmod_cond);

	_PUSH_LBL(Lmod_p1);
	_PUSH_ASMa("LOAD", Vt1);
	_PUSH_ASMa("SUB", Vt2);
	_PUSH_ASMa("STORE", Vt1);

	_PUSH_LBL(Lmod_p2);
	_PUSH_ASMa("LOAD", Vt2);
	_PUSH_ASMa("SUB", Vt3);
	_PUSH_ASMe("JZ", Lmod_end); // end
	_PUSH_ASMa("LOAD", Vt2);
	_PUSH_ASM("SHR"); // >> Vt2
	_PUSH_ASMa("STORE", Vt2);

	_PUSH_LBL(Lmod_cond);
	_PUSH_ASMa("LOAD", Vt2);
	_PUSH_ASMa("SUB", Vt1); // Vt2 - Vt1 <= 0 -> Vt1 >= Vt2
	_PUSH_ASMe("JZ", Lmod_p1);
	_PUSH_ASMe("JUMP", Lmod_p2);

    if(use_zero)
    {
        _PUSH_LBL(Lmod_zero);
        _PUSH_ASM("ZERO");
        _PUSH_ASMa("STORE", dest); // dest in ACC
    }
    _PUSH_LBL(Lmod_end);
    
    if(use_temp_dest) _PUSH_BLOCK(build_assign(dest, Vt1)); 

    return _;
}

ptr(code_block)
	snippet::build_if(ident cond_arg1, ident cond_arg2, int cond_type, ptr(code_block) cbif, ptr(code_block) cbelse)
{
    ptr(snippet) _(new snippet());
    _DECL_LBL(Lif_true);
    _DECL_LBL(Lif_false);
    _DECL_LBL(Lif_end);

    bool resolved = false;
    //condition here
    // comp to const:
    if(constants.count(cond_arg1) || constants.count(cond_arg2))
    {
        // both?
        if(constants.count(cond_arg1) && constants.count(cond_arg2))
        {
            // precompile:
            switch(cond_type)
            {
                case DEQUAL:
                    if(constants[cond_arg1] == constants[cond_arg2])
                        return cbif;
                    else
                        return cbelse;
                case NEQUAL:
                    if(constants[cond_arg1] != constants[cond_arg2])
                        return cbif;
                    else
                        return cbelse;
                case LESS:
                    if(constants[cond_arg1] < constants[cond_arg2])
                        return cbif;
                    else
                        return cbelse;
                case MORE:
                    if(constants[cond_arg1] > constants[cond_arg2])
                        return cbif;
                    else
                        return cbelse;
                case LESSEQUAL:
                    if(constants[cond_arg1] <= constants[cond_arg2])
                        return cbif;
                    else
                        return cbelse;
                case MOREEQUAL:
                    if(constants[cond_arg1] >= constants[cond_arg2])
                        return cbif;
                    else
                        return cbelse;
            }
        }

        ident cst = constants.count(cond_arg1) ? cond_arg1 : cond_arg2;
        ident var = constants.count(cond_arg1) ? cond_arg2 : cond_arg1;
        uint64 csv = constants[cst];

        if(!csv) // == 0
            switch(cond_type)
            {
                case DEQUAL:
                    _PUSH_ASMa("LOAD", var);
                    _PUSH_ASMe("JG", Lif_false);
                    resolved = true;
                    break;
                case NEQUAL:
                    _PUSH_ASMa("LOAD", var);
                    _PUSH_ASMe("JZ", Lif_false);
                    resolved = true;
                    break;
                case LESS:
                    if(cst == cond_arg1) // 0 < var
                    {
                        _PUSH_ASMa("LOAD", var);
                        _PUSH_ASMe("JZ", Lif_false);
                        resolved = true;
                        break;
                    }
                    else // var < 0
                        return cbelse;
                case MORE:
                    logme("GZ");
                    if(cst == cond_arg2) // var > 0
                    {
                        _PUSH_ASMa("LOAD", var);
                        _PUSH_ASMe("JZ", Lif_false);
                        resolved = true;
                        break;
                    }
                    else // 0 > var
                        return cbelse;
                case LESSEQUAL:
                    if(cst == cond_arg2) // var <= 0
                    {
                        _PUSH_ASMa("LOAD", var);
                        _PUSH_ASMe("JG", Lif_false);
                        resolved = true;
                        break;
                    }
                    else // 0 <= var
                        return cbif;
                case MOREEQUAL:
                    if(cst == cond_arg1) // 0 >= var
                    {
                        _PUSH_ASMa("LOAD", var);
                        _PUSH_ASMe("JG", Lif_false);
                        resolved = true;
                        break;
                    }
                    else // var >= 0
                        return cbif;
            }
    }
    
    if(!resolved)
    {
        switch(cond_type)
        {
            case DEQUAL:
                _PUSH_ASMa("LOAD", cond_arg1);
                _PUSH_ASMa("SUB", cond_arg2);
                _PUSH_ASMe("JG", Lif_false);
                _PUSH_ASMa("LOAD", cond_arg2);
                _PUSH_ASMa("SUB", cond_arg1);
                _PUSH_ASMe("JG", Lif_false);
                break;
            case NEQUAL:
                _PUSH_ASMa("LOAD", cond_arg1);
                _PUSH_ASMa("SUB", cond_arg2);
                _PUSH_ASMe("JG", Lif_true);
                _PUSH_ASMa("LOAD", cond_arg2);
                _PUSH_ASMa("SUB", cond_arg1);
                _PUSH_ASMe("JZ", Lif_false);
                break;
            case MORE:      // arg1 - arg2 > 0 -> arg1 > arg2!!!
            case LESSEQUAL:
                _PUSH_ASMa("LOAD", cond_arg1);
                _PUSH_ASMa("SUB", cond_arg2);
                if(cond_type == MORE)
                {
                    _PUSH_ASMe("JZ", Lif_false);
                }
                else
                {
                    _PUSH_ASMe("JG", Lif_false);
                }
                break;
            case LESS:      
            case MOREEQUAL:
                _PUSH_ASMa("LOAD", cond_arg2);
                _PUSH_ASMa("SUB", cond_arg1);
                if(cond_type == LESS)
                {
                    _PUSH_ASMe("JZ", Lif_false);
                }
                else
                {
                    _PUSH_ASMe("JG", Lif_false);
                }
                break;
        }
    }
    
    // a > b

    _PUSH_LBL(Lif_true);
    _PUSH_BLOCK(cbif);
    _PUSH_ASMe("JUMP", Lif_end);
    _PUSH_LBL(Lif_false);
    _PUSH_BLOCK(cbelse);
    _PUSH_LBL(Lif_end);
    return _;
}
ptr(code_block)
	snippet::build_while(ident cond_arg1, ident cond_arg2, int cond_type, ptr(code_block) blck)
{
    _DECL_LBL(Lwhile_start);
    
    // naive while by if:
    ptr(snippet) ift = new_ptr(snippet, blck), iff = new_ptr(snippet), _ = ift;

    _PUSH_ASMe("JUMP", Lwhile_start);

    _ = new_ptr(snippet);
    _PUSH_LBL(Lwhile_start);
    _PUSH_BLOCK(build_if(cond_arg1, cond_arg2, cond_type, ift, iff));
    
    _->is_loop = true;
    return _;
}

void snippet::push_block(ptr(code_block) cb)
{
	blocks.push_back(cb);
}

void snippet::push_block_front(ptr(code_block) cb)
{
	blocks.push_front(cb);
}

void snippet::add_remaining_constants()
{
    if(blocks.size() == 1 && blocks[0]->can_be_flattened && !blocks[0]->is_loop)
    // Immerse one level
    {
        std::dynamic_pointer_cast<snippet>(blocks[0])->add_remaining_constants();
    }
    else
    {
        auto vars = used_idents();
        
        for(auto cst : constants)
        {
            // if constant used at all:
            if(vars.count(cst.first))
            {
                // find the first block that uses it:
                for(auto it = blocks.begin(); it != blocks.end(); ++it)
                {
                    if((*it)->used_idents().count(cst.first))
                    {
                        blocks.insert(it, snippet::build_assign_num(cst.first, cst.second));
                        break;
                    }
                }
            }
        }
    }
}
