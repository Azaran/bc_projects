using System.Data.Entity;
using System.Data.Entity.Migrations;

namespace IW5Project.Services
{
    public class MoviesDbMigrationsConfiguration<T> : DbMigrationsConfiguration<T>
        where T : DbContext
    {
        public MoviesDbMigrationsConfiguration()
        {
            this.AutomaticMigrationsEnabled = true;
            this.AutomaticMigrationDataLossAllowed = true;
        }

    }
}