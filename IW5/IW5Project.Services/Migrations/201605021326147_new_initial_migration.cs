namespace IW5Project.Services.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class new_initial_migration : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "dbo.Movies",
                c => new
                    {
                        Id = c.Guid(nullable: false),
                        OrigName = c.String(),
                        Name = c.String(),
                        Genre = c.String(),
                        PosterEnding = c.String(),
                        IsPosterSet = c.Boolean(nullable: false),
                        Country = c.String(),
                        Length = c.Int(nullable: false),
                        Description = c.String(),
                        AvrRating = c.Int(nullable: false),
                    })
                .PrimaryKey(t => t.Id);
            
            CreateTable(
                "dbo.PersonMovieRoles",
                c => new
                    {
                        Id = c.Guid(nullable: false),
                        WorkRole = c.Int(nullable: false),
                        Movie_Id = c.Guid(),
                        Person_Id = c.Guid(),
                    })
                .PrimaryKey(t => t.Id)
                .ForeignKey("dbo.Movies", t => t.Movie_Id)
                .ForeignKey("dbo.Persons", t => t.Person_Id)
                .Index(t => t.Movie_Id)
                .Index(t => t.Person_Id);
            
            CreateTable(
                "dbo.Persons",
                c => new
                    {
                        Id = c.Guid(nullable: false),
                        Firstname = c.String(),
                        Lastname = c.String(),
                        Avatar = c.String(),
                        Age = c.Int(nullable: false),
                        AvatarEnding = c.String(),
                        IsPosterSet = c.Boolean(nullable: false),
                    })
                .PrimaryKey(t => t.Id);
            
            CreateTable(
                "dbo.Ratings",
                c => new
                    {
                        Id = c.Guid(nullable: false),
                        Stars = c.Int(nullable: false),
                        Comment = c.String(),
                        Movie_Id = c.Guid(),
                    })
                .PrimaryKey(t => t.Id)
                .ForeignKey("dbo.Movies", t => t.Movie_Id)
                .Index(t => t.Movie_Id);
            
        }
        
        public override void Down()
        {
            DropForeignKey("dbo.Ratings", "Movie_Id", "dbo.Movies");
            DropForeignKey("dbo.PersonMovieRoles", "Person_Id", "dbo.Persons");
            DropForeignKey("dbo.PersonMovieRoles", "Movie_Id", "dbo.Movies");
            DropIndex("dbo.Ratings", new[] { "Movie_Id" });
            DropIndex("dbo.PersonMovieRoles", new[] { "Person_Id" });
            DropIndex("dbo.PersonMovieRoles", new[] { "Movie_Id" });
            DropTable("dbo.Ratings");
            DropTable("dbo.Persons");
            DropTable("dbo.PersonMovieRoles");
            DropTable("dbo.Movies");
        }
    }
}
