enum object_file_type{
  FT_UNK = 0,
  FT_BITCODE = 1,
  FT_OBJECT  = 2
};

int compile(const char* fn, object_file_type file_type);