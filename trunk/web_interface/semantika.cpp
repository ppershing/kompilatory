// vim: set fdm=marker:
#include <stdio.h>
#include <vector>
#include <string>
using namespace std;


class SemantikaData {
public:
int global_id;
vector<int> terminal_nodes;
FILE* out;
};

class Metadata {
public:
int node_id;
string node_string;
};

// {{{ debug semantika vypisujuca co ma robit
void semantika_initialize_global_data_debug() {
  printf("SemantikaInitialize:\n");
}

void semantika_finalize_debug(SemantikaData* global_data) {
  printf("SemantikaFinalize:\n");
  printf("passed data: ptr %x\n", global_data);
  printf("\n");
}

void get_metadata_terminal_symbol_debug(SemantikaData* global_data, int symbol) {
  printf("Semantika - get metadata for terminal symbol:\n");
  printf("returning new metadata terminal for %d %c \n", symbol, (char) symbol);
  printf("\n");
}

void get_metadata_reduce_debug(SemantikaData* global_data,int semantic_number, int rhs_size, char* lhs, char** rhs_symbols, Metadata** metadata) {
  printf("Semantika reduce: (semantic %d), rhs size %d\n", semantic_number, rhs_size);
  printf("LHS:  '%s'  ----->\n", lhs);

  for (int i=0; i<rhs_size; i++) {
    printf("      %d: symbol '%s', meta %d\n",i,  rhs_symbols[i], metadata[i]);
  }

  printf("will return new metadata ...\n");
  printf("\n");
  return;
}

void semantika_accept_debug(SemantikaData* global_data, Metadata* metadata) {
  printf("Semantika accept, start symbol have metadata %d\n", metadata);
  printf("!!!!!!!!!!!!!!!!!!!!!!ACCEPT!!!!!!!!!!!!!!!!!!\n");
  printf("\n");
}

// }}}

// {{{ semantika_initialize_global_data
extern "C"
SemantikaData* semantika_initialize_global_data() {
  semantika_initialize_global_data_debug();

    SemantikaData *data = new SemantikaData();
    printf ("Initializing global data : ptr %p\n", data);
    printf("\n");

    data->global_id = 0;
    data->out = fopen("graph.dot", "w");
    fprintf(data->out, "digraph {\n");
    fprintf(data->out, "start [shape=Mdiamond];\n");
    return data;
}
// }}}

// {{{ semantika_finalize
extern "C"
int semantika_finalize(SemantikaData* global_data) {
  semantika_finalize_debug(global_data);
  delete global_data;
  return 0;
}
// }}}

// {{{ get_metadata_terminal_symbol
extern "C"
Metadata* get_metadata_terminal_symbol(SemantikaData* global_data, int symbol) {
  get_metadata_terminal_symbol_debug(global_data, symbol);
  Metadata* meta = new Metadata();

  char tmp[20];
  if (symbol>=32 && symbol!='"' && symbol!='\\') {
    sprintf(tmp, "'%c'", symbol);
  } else {
    sprintf(tmp, "[%d]", symbol);
  }
  meta->node_string = tmp;
  fprintf(global_data->out, "node_%d [ label = \"%s\", "
          "shape = box, style=filled, color=greenyellow, fontsize=20];\n",
        global_data->global_id, tmp);

  global_data->terminal_nodes.push_back(global_data->global_id);
  meta->node_id = global_data->global_id++;
  return meta;
}
// }}}


// {{{ get_metadata_reduce
extern "C"
Metadata* get_metadata_reduce(SemantikaData* global_data, int semantic_number, int rhs_size, char* lhs, char** rhs_symbols, Metadata** metadata) {
  get_metadata_reduce_debug(global_data, semantic_number, rhs_size, lhs, rhs_symbols, metadata);
  
  Metadata* meta = new Metadata();

  string reduced_string = "";
  string rule_label_string = lhs;
  rule_label_string += " ->";

  for (int i=0; i<rhs_size; i++) {

    if (i) reduced_string += ", ";
    reduced_string += metadata[i]->node_string;
    rule_label_string += " ";
    rule_label_string += rhs_symbols[i];
  }

  fprintf(global_data->out, "node_%d [ label = \"%s\", style = filled ];\n", global_data->global_id,
			rule_label_string.c_str());
 
  for (int i=0; i<rhs_size; i++) {
    fprintf(global_data->out, "node_%d -> node_%d "
        "[ label = \"%s\", fontsize=20 ];\n",
            global_data->global_id, metadata[i]->node_id, rhs_symbols[i]);


          //rule_label_string.c_str());


    delete metadata[i];
  }



  meta->node_string = reduced_string ;
  meta->node_id = global_data->global_id++;
  return meta;
}
// }}}

// {{{ semantika_accept
extern "C"
void semantika_accept(SemantikaData* global_data, Metadata* metadata) {
  semantika_accept_debug(global_data, metadata);
  fprintf(global_data->out, "{ rank = same;\n");
  for (int i = 0; i < global_data->terminal_nodes.size(); i++) {
    fprintf(global_data->out, "node_%d;\n", global_data->terminal_nodes[i]);
  }
  fprintf(global_data->out, "}\n");

  fprintf(global_data->out, "start -> node_%d; \n", global_data->global_id-1);
  fprintf(global_data->out, "}\n");
  fclose(global_data->out);
}
// }}}
