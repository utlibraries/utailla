<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns="http://www.loc.gov/mods/v3" xmlns:mods="http://www.loc.gov/mods/v3">
    <xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes" media-type="text/xml"/>
    <xsl:strip-space elements="*"/>
    <xsl:variable name="ailla-role-fields" select="document('ailla_genre_roles.xml')"/>

    <xsl:template match="node()|@*">
        <xsl:copy>
            <xsl:apply-templates select="node()|@*"/>
        </xsl:copy>
    </xsl:template>

    <!-- Remove all Spanish roles and re-transform to handle CRUD -->
    <xsl:template match='/mods:mods/mods:name[@authority="aillaPerson"]/mods:role[mods:roleTerm[@lang="spa" and @authority="aillaRoleTerms"]]'/>
    <xsl:template match='/mods:mods/mods:name[@authority="aillaPerson"]/mods:role[mods:roleTerm[@lang="eng" and @authority="aillaRoleTerms"]]'>
        <xsl:variable name="english_role" select="mods:roleTerm/text()"/>
        <xsl:copy-of select="*/.."/>
        <role>
            <roleTerm type="text" lang="spa" authority="aillaRoleTerms">
                <xsl:value-of select="$ailla-role-fields/terms/roles/term[english[text() = $english_role]]/spanish"/>
            </roleTerm>
        </role>
    </xsl:template>

    <!-- Remove all Spanish genres and re-transform to handle CRUD -->
    <xsl:template match='/mods:mods/mods:genre[@type="local" and @authority="aillaResourceGenres" and @lang="spa"]'/>
    <xsl:template match='/mods:mods/mods:genre[@type="local" and @authority="aillaResourceGenres" and @lang="eng"]'>
        <xsl:variable name="english_genre" select="text()"/>
        <xsl:copy-of select="current()"/>
        <genre type="local" authority="aillaResourceGenres" lang="spa">
            <xsl:value-of select="$ailla-role-fields/terms/genres/term[english[text() = $english_genre]]/spanish"/>
        </genre>
    </xsl:template>
</xsl:stylesheet>
