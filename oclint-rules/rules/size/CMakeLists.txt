SET(LIST_OF_RULES
    CyclomaticComplexity
    LongClass
    LongLine
    LongMethod
    NcssMethodCount
    NestedBlockDepth
    NPathComplexity
    TailleFichier
    TooManyFields
    TooManyMethods
    TooManyParameters
    NombreAppelsProcedures
    FrequenceCommentaires
    NbreSortiesParFonction
    )

BUILD_DYNAMIC_RULES("${LIST_OF_RULES}")
