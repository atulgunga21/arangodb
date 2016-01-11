////////////////////////////////////////////////////////////////////////////////
/// @startDocuBlock documentsDocumentExists
/// @brief checks whether a document exists
/// `db._exists(document)`
///
/// This method determines whether a document exists given its identifier.
/// Instead of returning the found document or an error, this method will
/// return either *true* or *false*. It can thus be used
/// for easy existence checks.
///
/// No error will be thrown if the sought document or collection does not
/// exist.
/// Still this method will throw an error if used improperly, e.g. when called
/// with a non-document handle.
///
/// `db._exists(document-handle)`
///
/// As before, but instead of a document a document-handle can be passed.
/// @endDocuBlock
////////////////////////////////////////////////////////////////////////////////